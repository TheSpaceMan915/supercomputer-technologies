// assignment5.cpp — MPI row-block matrix multiply (C++98, MPI-1 compatible)
//
// Build (Linux/macOS, Open MPI):
//   mpic++ -std=c++98 -O2 -Wall -Wextra -Wpedantic -o assignment5 assignment5.cpp
//
// Build (Windows/Cygwin, Open MPI):
//   mpic++ -std=c++98 -O2 -Wall -Wextra -Wpedantic -o assignment5 assignment5.cpp
//
// Run (2 ranks example):
//   mpirun -np 2 ./assignment5 1024 --iters 3
//
// Notes:
// - Uses MPI-1 API only (MPI_Bcast, MPI_Send/Recv, MPI_Barrier, MPI_Wtime).
// - Row-block distribution: each rank owns a contiguous block of rows of A and C.
// - B is filled on rank 0 and broadcast once to all ranks (outside the timed region).

#include <mpi.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <stdexcept>
#include <limits>

static const int TAG_C00    = 100;
static const int TAG_C0N1   = 101;
static const int TAG_CN10   = 102;
static const int TAG_CN1N1  = 103;

// ---------- Tiny logging helpers ----------
static void log_info_root(int rank, const std::string& msg) {
    if (rank == 0) std::cout << msg << std::endl;
}
static void log_error_any(int rank, const std::string& msg) {
    // All ranks may print errors to help diagnose distributed failures
    std::cerr << msg << std::endl;
}

// ---------- CLI parsing ----------
static void print_usage() {
    std::cerr << "Usage: assignment5 <N> [--iters K]" << std::endl;
}

// Parse args: <N> required, optional --iters K (default 1). Returns true on success.
static bool parse_args(int argc, char** argv, int& N, int& iters, int rank) {
    N = 0;
    iters = 1;

    if (argc < 2) {
        log_error_any(rank, "[ERROR] missing <N> argument");
        print_usage();
        return false;
    }

    // First positional: N
    {
        char* endp = 0;
        long v = std::strtol(argv[1], &endp, 10);
        if (endp == argv[1] || *endp != '\0') {
            log_error_any(rank, std::string("[ERROR] invalid N: ") + argv[1]);
            print_usage();
            return false;
        }
        if (v <= 0 || v > static_cast<long>(std::numeric_limits<int>::max())) {
            log_error_any(rank, "[ERROR] N must be a positive int");
            print_usage();
            return false;
        }
        N = static_cast<int>(v);
    }

    // Optional flags
    int i = 2;
    while (i < argc) {
        std::string arg(argv[i]);
        if (arg == "--iters") {
            if (i + 1 >= argc) {
                log_error_any(rank, "[ERROR] --iters requires a value");
                print_usage();
                return false;
            }
            char* endp = 0;
            long v = std::strtol(argv[i + 1], &endp, 10);
            if (endp == argv[i + 1] || *endp != '\0' || v <= 0 || v > static_cast<long>(std::numeric_limits<int>::max())) {
                log_error_any(rank, std::string("[ERROR] invalid iters: ") + argv[i + 1]);
                print_usage();
                return false;
            }
            iters = static_cast<int>(v);
            i += 2;
        } else {
            std::ostringstream oss;
            oss << "[ERROR] unknown option: " << arg;
            log_error_any(rank, oss.str());
            print_usage();
            return false;
        }
    }

    // Conservative memory guard for B (N*N doubles). Avoid overflow: check before multiplying.
    // bytes = N * N * sizeof(double); guard: N > SIZE_MAX / N -> overflow
    const std::size_t sN = static_cast<std::size_t>(N);
    if (sN != 0 && sN > std::numeric_limits<std::size_t>::max() / sN) {
        log_error_any(rank, "[ERROR] N*N overflows size limits");
        return false;
    }
    const std::size_t elemsB = sN * sN;
    const std::size_t bytesB = elemsB * sizeof(double);
    const std::size_t THRESHOLD = (static_cast<std::size_t>(1) << 30); // ~1 GiB
    if (bytesB > THRESHOLD) {
        std::ostringstream oss;
        oss << "[ERROR] requested B size " << (bytesB / (1024.0*1024.0)) << " MiB exceeds conservative threshold (~1024 MiB).";
        log_error_any(rank, oss.str());
        return false;
    }

    return true;
}

// ---------- Row partitioning (row-block) ----------
// Compute [row0, row1) for this rank among P ranks over N rows.
// Remainder distributed to the first 'rem' ranks.
static void compute_row_partition(int N, int P, int rank, int& row0, int& row1) {
    const int base = N / P;
    const int rem  = N % P;
    const int extra = (rank < rem) ? 1 : 0;
    const int start = rank * base + ((rank < rem) ? rank : rem);
    row0 = start;
    row1 = start + base + extra;
}

// Compute which rank owns a global row index
static int owner_of_row(int N, int P, int row) {
    // Mirror logic of compute_row_partition
    const int base = N / P;
    const int rem  = N % P;
    // For rows in the first (base+1)*rem, size is base+1; afterward, size is base
    // We can search by threshold
    const int split = (base + 1) * rem;
    if (row < split) {
        // rows of size base+1
        return row / (base + 1);
    } else {
        // rows of size base
        return rem + (row - split) / base;
    }
}

// ---------- Matrix B init and broadcast ----------
static void fill_B(std::vector<double>& B, int N) {
    // B[i][j] = 1.0 / (j+1)
    // Stored row-major 1D: idx = i*N + j
    for (int i = 0; i < N; ++i) {
        const int base = i * N;
        for (int j = 0; j < N; ++j) {
            B[base + j] = 1.0 / static_cast<double>(j + 1);
        }
    }
}

static bool bcast_B(std::vector<double>& B, int N) {
    const int rc = MPI_Bcast((B.empty() ? NULL : &B[0]), N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    return rc == MPI_SUCCESS;
}

// ---------- Local compute over this rank's row block ----------
// Cloc size: (row1-row0) * N
static void compute_block(const std::vector<double>& B,
                          std::vector<double>& Cloc,
                          int N, int row0, int row1)
{
    const int local_rows = row1 - row0;
    // C local is laid out as local_rows x N
    for (int ii = 0; ii < local_rows; ++ii) {
        const int ig = row0 + ii;              // global row index
        const double a_row_value = static_cast<double>(ig) + 1.0; // since A[i][k] = i+1
        double* c_row = (local_rows == 0 ? (double*)0 : &Cloc[ii * N]);
        for (int j = 0; j < N; ++j) {
            // Compute dot( A_row, B_col )
            // A_row[k] = a_row_value (constant over k), so sum = (i+1) * sum_k B[k*N+j].
            double sum = 0.0;
            for (int k = 0; k < N; ++k) {
                sum += a_row_value * B[k * N + j];
            }
            c_row[j] = sum;
        }
    }
}

// Send boundary values (if owned) to rank 0.
static void send_boundary_values_to_root(const std::vector<double>& Cloc,
                                         int N, int row0, int row1,
                                         int rank)
{
    const int local_rows = row1 - row0;
    if (local_rows <= 0) return;

    // (0,0) and (0,N-1)
    if (0 >= row0 && 0 < row1) {
        const int li = 0 - row0;
        const double c00  = Cloc[li * N + 0];
        const double c0N1 = Cloc[li * N + (N - 1)];
        if (rank != 0) {
            MPI_Send((void*)&c00,  1, MPI_DOUBLE, 0, TAG_C00,  MPI_COMM_WORLD);
            MPI_Send((void*)&c0N1, 1, MPI_DOUBLE, 0, TAG_C0N1, MPI_COMM_WORLD);
        }
        // If rank==0, root will read from its own Cloc directly
    }

    // (N-1,0) and (N-1,N-1)
    if ((N - 1) >= row0 && (N - 1) < row1) {
        const int li = (N - 1) - row0;
        const double cN10  = Cloc[li * N + 0];
        const double cN1N1 = Cloc[li * N + (N - 1)];
        if (rank != 0) {
            MPI_Send((void*)&cN10,  1, MPI_DOUBLE, 0, TAG_CN10,  MPI_COMM_WORLD);
            MPI_Send((void*)&cN1N1, 1, MPI_DOUBLE, 0, TAG_CN1N1, MPI_COMM_WORLD);
        }
        // If rank==0, root will read from its own Cloc directly
    }
}

// Root (rank 0) collects four boundary elements from owners and logs them
static void collect_and_log_boundaries_root(const std::vector<double>& Cloc,
                                            int N, int row0, int row1,
                                            int P)
{
    double c00 = 0.0, c0N1 = 0.0, cN10 = 0.0, cN1N1 = 0.0;

    const int owner_r0   = owner_of_row(N, P, 0);
    const int owner_rN1  = owner_of_row(N, P, N - 1);

    MPI_Status st;

    // Get (0,0) and (0,N-1)
    if (owner_r0 == 0) {
        const int li = 0 - row0;
        c00  = Cloc[li * N + 0];
        c0N1 = Cloc[li * N + (N - 1)];
    } else {
        MPI_Recv((void*)&c00,  1, MPI_DOUBLE, owner_r0, TAG_C00,  MPI_COMM_WORLD, &st);
        MPI_Recv((void*)&c0N1, 1, MPI_DOUBLE, owner_r0, TAG_C0N1, MPI_COMM_WORLD, &st);
    }

    // Get (N-1,0) and (N-1,N-1)
    if (owner_rN1 == 0) {
        const int li = (N - 1) - row0;
        cN10  = Cloc[li * N + 0];
        cN1N1 = Cloc[li * N + (N - 1)];
    } else {
        MPI_Recv((void*)&cN10,  1, MPI_DOUBLE, owner_rN1, TAG_CN10,  MPI_COMM_WORLD, &st);
        MPI_Recv((void*)&cN1N1, 1, MPI_DOUBLE, owner_rN1, TAG_CN1N1, MPI_COMM_WORLD, &st);
    }

    std::ostringstream oss;
    oss.setf(std::ios::fixed); oss.precision(10);
    oss << "[INFO] C[0][0]=" << c00
        << " C[0][" << (N-1) << "]=" << c0N1
        << " C[" << (N-1) << "][0]=" << cN10
        << " C[" << (N-1) << "][" << (N-1) << "]=" << cN1N1;
    log_info_root(0, oss.str());
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int P = 1, rank = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &P);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int N = 0, iters = 1;
    if (!parse_args(argc, argv, N, iters, rank)) {
        // Ensure all ranks exit; avoid deadlock if some waited for collectives.
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    // Partition rows
    int row0 = 0, row1 = 0;
    compute_row_partition(N, P, rank, row0, row1);
    const int local_rows = row1 - row0;

    // Allocate B on all ranks (guard local memory as well)
    std::vector<double> B;
    try {
        B.resize(static_cast<std::size_t>(N) * static_cast<std::size_t>(N));
    } catch (const std::bad_alloc&) {
        log_error_any(rank, "[ERROR] allocation failed for B");
        MPI_Abort(MPI_COMM_WORLD, 2);
        return 2;
    }

    // Fill B on root and broadcast once (outside timing)
    if (rank == 0) fill_B(B, N);
    if (!bcast_B(B, N)) {
        log_error_any(rank, "[ERROR] MPI_Bcast failed for B");
        MPI_Abort(MPI_COMM_WORLD, 3);
        return 3;
    }

    // Allocate local C
    std::vector<double> Cloc;
    try {
        Cloc.resize(static_cast<std::size_t>(local_rows) * static_cast<std::size_t>(N));
    } catch (const std::bad_alloc&) {
        log_error_any(rank, "[ERROR] allocation failed for local C");
        MPI_Abort(MPI_COMM_WORLD, 4);
        return 4;
    }

    // Start banner
    {
        std::ostringstream oss;
        oss << "[INFO] assignment5 start";
        log_info_root(rank, oss.str());
        std::ostringstream oss2;
        oss2 << "[INFO] N=" << N << " iters=" << iters << " ranks=" << P << " dist=row-block";
        log_info_root(rank, oss2.str());
    }

    // Synchronize before timing to start from a clean epoch
    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    // Timed compute: repeat full multiply iters times (average time is reported)
    for (int rep = 0; rep < iters; ++rep) {
        if (!Cloc.empty()) std::fill(Cloc.begin(), Cloc.end(), 0.0);
        compute_block(B, Cloc, N, row0, row1);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double t1 = MPI_Wtime();

    // Send boundary scalars to root
    send_boundary_values_to_root(Cloc, N, row0, row1, rank);

    // Root collects and logs boundary elements
    if (rank == 0) {
        collect_and_log_boundaries_root(Cloc, N, row0, row1, P);

        const double total_seconds = (t1 - t0);
        const double avg_seconds   = (iters > 0) ? (total_seconds / static_cast<double>(iters)) : 0.0;
        const double elapsed_ms    = avg_seconds * 1000.0;

        const double flops  = 2.0 * static_cast<double>(N) * static_cast<double>(N) * static_cast<double>(N);
        const double gflops = (avg_seconds > 0.0) ? (flops / (avg_seconds * 1e9)) : 0.0;

        std::ostringstream ossE;
        ossE.setf(std::ios::fixed); ossE.precision(2);
        std::ostringstream ossF; ossF.setf(std::ios::scientific); ossF.precision(3);

        // Keep FLOPS in scientific to avoid huge integers; GFLOPS fixed with 3 decimals
        ossF.str(""); ossF.clear();
        ossF << flops;

        std::ostringstream final;
        final.setf(std::ios::fixed);
        final.precision(2);
        final << "[INFO] elapsed_ms=" << elapsed_ms
              << " flops=" << ossF.str()
              << " gflops=" << ( (avg_seconds > 0.0) ? (flops / (avg_seconds * 1e9)) : 0.0 );
        log_info_root(0, final.str());

        log_info_root(0, "[INFO] assignment5 done");
    }

    MPI_Finalize();
    return 0;
}
