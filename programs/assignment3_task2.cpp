// assignment3_task2.cpp
//
// Build with OpenMP (Linux/macOS, GCC/Clang):
//   g++ -std=c++98 -O2 -fopenmp -Wall -Wextra -o assignment3_task2 assignment3_task2.cpp
//
// Build without OpenMP:
//   g++ -std=c++98 -O2 -Wall -Wextra -Wpedantic -o assignment3_task2 assignment3_task2.cpp
//
// Build with MSVC (OpenMP):
//   cl /EHsc /W4 /openmp assignment3_task2.cpp /Fe:assignment3_task2.exe
//
// Run:
//   ./assignment3_task2 512
//
// Single-translation-unit version of assignment3-task2.
// Computes C = A · B for N×N matrices with:
//   A[i][j] = i + 1
//   B[i][j] = 1.0 / (j + 1)
// Uses std::vector<double> and classic triple-loop multiplication.
// If compiled with OpenMP, parallelizes the outer loop; otherwise runs serial.
// Logs N, mode, key C elements, elapsed time, FLOPS, and GFLOPS.

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <climits>
#include <ctime>
#include <new>

#ifdef _OPENMP
#include <omp.h>
#endif

// Forward declarations
bool parse_n(int argc, char** argv, int& n);
bool is_n_reasonable(int n);
void init_matrices(std::vector<double>& A, std::vector<double>& B, int N);
void multiply_serial(const std::vector<double>& A,
                     const std::vector<double>& B,
                     std::vector<double>& C,
                     int N);
void multiply_parallel(const std::vector<double>& A,
                       const std::vector<double>& B,
                       std::vector<double>& C,
                       int N);

void log_info(const char* msg);
void log_info(const std::string& msg);
void log_error(const std::string& msg);
void print_usage();

double now_seconds();
int get_thread_count();

// Parse and validate N from command line.
bool parse_n(int argc, char** argv, int& n)
{
    if (argc != 2)
    {
        log_error("invalid argument count");
        print_usage();
        return false;
    }

    const char* s = argv[1];
    if (!s || *s == '\0')
    {
        log_error("invalid N: missing value");
        print_usage();
        return false;
    }

    errno = 0;
    char* endp = 0;
    long val = std::strtol(s, &endp, 10);

    if (errno == ERANGE)
    {
        log_error(std::string("invalid N: out of range: ") + s);
        print_usage();
        return false;
    }
    if (endp == s || *endp != '\0')
    {
        log_error(std::string("invalid N: not an integer: ") + s);
        print_usage();
        return false;
    }
    if (val <= 0)
    {
        log_error("invalid N: must be positive");
        print_usage();
        return false;
    }
    if (val > INT_MAX)
    {
        log_error("invalid N: exceeds INT_MAX");
        print_usage();
        return false;
    }

    n = static_cast<int>(val);

    if (!is_n_reasonable(n))
    {
        // is_n_reasonable already logged an error message.
        return false;
    }

    return true;
}

// Basic sanity check for N to avoid absurd memory/time usage.
bool is_n_reasonable(int n)
{
    if (n <= 0)
    {
        log_error("invalid N: must be positive");
        return false;
    }

    // Approximate memory usage for A, B, C:
    // 3 * N * N * sizeof(double) bytes.
    const double bytes =
        3.0 * static_cast<double>(n) *
        static_cast<double>(n) *
        static_cast<double>(sizeof(double));

    const double limit = 1024.0 * 1024.0 * 1024.0; // ~1 GiB

    if (bytes > limit)
    {
        log_error("N too large: estimated memory usage exceeds ~1 GiB");
        return false;
    }

    // Also guard against N*N overflow in 32-bit int indexing assumptions.
    if (n > 46340) // since 46340^2 < 2^31-1
    {
        log_error("N too large: may overflow 32-bit index computations");
        return false;
    }

    return true;
}

// Initialize A and B with the specified formulas.
void init_matrices(std::vector<double>& A, std::vector<double>& B, int N)
{
    const int size = N * N;
    A.resize(size);
    B.resize(size);

    // A[i][j] = i + 1
    for (int i = 0; i < N; ++i)
    {
        const double value = static_cast<double>(i + 1);
        const int row = i * N;
        for (int j = 0; j < N; ++j)
        {
            A[row + j] = value;
        }
    }

    // B[i][j] = 1.0 / (j + 1)
    for (int j = 0; j < N; ++j)
    {
        const double value = 1.0 / static_cast<double>(j + 1);
        for (int i = 0; i < N; ++i)
        {
            B[i * N + j] = value;
        }
    }
}

// Serial naive matrix multiply: C = A · B.
void multiply_serial(const std::vector<double>& A,
                     const std::vector<double>& B,
                     std::vector<double>& C,
                     int N)
{
    const int size = N * N;
    C.assign(size, 0.0);

    for (int i = 0; i < N; ++i)
    {
        const int row = i * N;
        for (int j = 0; j < N; ++j)
        {
            double sum = 0.0;
            for (int k = 0; k < N; ++k)
            {
                sum += A[row + k] * B[k * N + j];
            }
            C[row + j] = sum;
        }
    }
}

// Parallel multiply: uses OpenMP when available; otherwise calls serial.
void multiply_parallel(const std::vector<double>& A,
                       const std::vector<double>& B,
                       std::vector<double>& C,
                       int N)
{
    const int size = N * N;
    C.assign(size, 0.0);

#if defined(_OPENMP)
    // Parallelize over rows.
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < N; ++i)
    {
        const int row = i * N;
        for (int j = 0; j < N; ++j)
        {
            double sum = 0.0;
            for (int k = 0; k < N; ++k)
            {
                sum += A[row + k] * B[k * N + j];
            }
            C[row + j] = sum;
        }
    }
#else
    // No OpenMP: fall back to serial implementation.
    multiply_serial(A, B, C, N);
#endif
}

// Logging helpers

void log_info(const char* msg)
{
    std::cout << "[INFO] " << msg << std::endl;
}

void log_info(const std::string& msg)
{
    std::cout << "[INFO] " << msg << std::endl;
}

void log_error(const std::string& msg)
{
    std::fprintf(stderr, "[ERROR] %s\n", msg.c_str());
}

void print_usage()
{
    std::fprintf(stderr, "Usage: assignment3_task2 <N>\n");
}

// Time in seconds, using OpenMP when available, else std::clock().
double now_seconds()
{
#ifdef _OPENMP
    return omp_get_wtime();
#else
    const std::clock_t c = std::clock();
    return static_cast<double>(c) / static_cast<double>(CLOCKS_PER_SEC);
#endif
}

// Get number of threads (or 1 if OpenMP is not enabled).
int get_thread_count()
{
#ifdef _OPENMP
    int t = omp_get_max_threads();
    if (t < 1)
    {
        t = 1;
    }
    return t;
#else
    return 1;
#endif
}

int main(int argc, char** argv)
{
    int N = 0;
    if (!parse_n(argc, argv, N))
    {
        return 1;
    }

    log_info("assignment3-task2 start");

    {
        std::ostringstream oss;
        oss << "N=" << N;
        log_info(oss.str());
    }

    const bool has_openmp =
#ifdef _OPENMP
        true;
#else
        false;
#endif

    if (has_openmp)
    {
        std::ostringstream oss;
        oss << "mode=parallel threads=" << get_thread_count();
        log_info(oss.str());
    }
    else
    {
        log_info("mode=serial");
    }

    std::vector<double> A;
    std::vector<double> B;
    std::vector<double> C;

    try
    {
        init_matrices(A, B, N);
        C.resize(N * N);
    }
    catch (const std::bad_alloc&)
    {
        log_error("allocation failed");
        return 1;
    }

    const double t0 = now_seconds();

    if (has_openmp)
    {
        multiply_parallel(A, B, C, N);
    }
    else
    {
        multiply_serial(A, B, C, N);
    }

    const double t1 = now_seconds();
    const double elapsed_s = (t1 > t0) ? (t1 - t0) : 0.0;
    const double elapsed_ms = elapsed_s * 1000.0;

    // Log boundary elements of C.
    if (N > 0)
    {
        const int last = N - 1;
        const double c00 = C[0 * N + 0];
        const double c0L = C[0 * N + last];
        const double cL0 = C[last * N + 0];
        const double cLL = C[last * N + last];

        std::ostringstream oss;
        oss.setf(std::ios::fixed);
        oss.precision(6);
        oss << "C[0][0]=" << c00
            << " C[0][" << last << "]=" << c0L
            << " C[" << last << "][0]=" << cL0
            << " C[" << last << "][" << last << "]=" << cLL;
        log_info(oss.str());
    }

    // Compute FLOPS and GFLOPS for naive triple-loop: 2 * N^3 operations.
    const double n_d = static_cast<double>(N);
    const double flops = 2.0 * n_d * n_d * n_d;

    double gflops = 0.0;
    if (elapsed_s > 0.0)
    {
        gflops = flops / (elapsed_s * 1e9);
    }

    {
        std::ostringstream oss;
        oss.setf(std::ios::fixed);
        oss.precision(2);
        oss << "elapsed_ms=" << elapsed_ms;

        oss << " flops=";
        oss.setf(std::ios::scientific, std::ios::floatfield);
        oss.precision(6);
        oss << flops;

        oss.setf(std::ios::fixed, std::ios::floatfield);
        oss.precision(2);
        oss << " gflops=";
        if (elapsed_s > 0.0)
        {
            oss << gflops;
        }
        else
        {
            oss << 0.0; // No time measured; treat as 0 safely.
        }

        log_info(oss.str());
    }

    log_info("assignment3-task2 done");
    return 0;
}