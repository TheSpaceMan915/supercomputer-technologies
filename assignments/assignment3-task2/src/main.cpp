/* main.cpp: Command-line driver for parallel matrix multiplication benchmark.
 * Parses N from argv, initializes A and B, performs multiplication, and reports timing.
 * Uses OpenMP for timing and parallelization when available; falls back to serial otherwise.
 */
#include "assignment3_task2/matrix.h"
#include "assignment3_task2/logger.h"

#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <climits>
#include <ctime>
#include <new>

#ifdef _OPENMP
#include <omp.h>
#endif

using assignment3_task2::log_info;
using assignment3_task2::log_error;

static void print_usage()
{
    std::fprintf(stderr, "Usage: assignment3-task2 <N>\n");
}

// Parse and validate N from command-line arguments.
// Returns false on error (prints diagnostic and usage).
// Enforces: N > 0, N <= INT_MAX, and 3*N*N*sizeof(double) <= 1 GiB.
static bool parse_N(int argc, char** argv, int& N)
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
        log_error("missing N");
        print_usage();
        return false;
    }

    errno = 0;
    char* endp = 0;
    long val = std::strtol(s, &endp, 10);

    if (errno == ERANGE)
    {
        log_error(std::string("invalid N (out of range): ") + s);
        print_usage();
        return false;
    }
    if (endp == s || *endp != '\0')
    {
        log_error(std::string("invalid N (not an integer): ") + s);
        print_usage();
        return false;
    }
    if (val <= 0)
    {
        log_error("N must be positive");
        print_usage();
        return false;
    }

    // Prevent excessive memory allocation: 3 N×N matrices must fit in 1 GiB.
    const double bytes =
        3.0 * static_cast<double>(val) * static_cast<double>(val) *
        static_cast<double>(sizeof(double));
    const double limit = 1024.0 * 1024.0 * 1024.0;

    if (bytes > limit)
    {
        log_error("N too large for safe allocation; choose a smaller N");
        return false;
    }

    if (val > INT_MAX)
    {
        log_error("N exceeds INT_MAX");
        return false;
    }

    N = static_cast<int>(val);
    return true;
}

// High-resolution wall-clock timer.
// Uses omp_get_wtime() when OpenMP is available; otherwise std::clock().
static double now_seconds()
{
#ifdef _OPENMP
    return omp_get_wtime();  // More accurate wall-clock time
#else
    const std::clock_t c = std::clock();
    return static_cast<double>(c) / static_cast<double>(CLOCKS_PER_SEC);
#endif
}

int main(int argc, char** argv)
{
    int N = 0;
    if (!parse_N(argc, argv, N))
    {
        return 1;
    }

    log_info("assignment3-task2 start");

    {
        std::ostringstream oss;
        oss << "N=" << N;
        log_info(oss.str());
    }

    // Detect OpenMP availability at compile time.
    bool parallel = false;
#ifdef _OPENMP
    parallel = true;
#endif

    if (parallel)
    {
#ifdef _OPENMP
        int threads = omp_get_max_threads();  // Respects OMP_NUM_THREADS
        if (threads < 1) { threads = 1; }
        std::ostringstream oss;
        oss << "mode=parallel threads=" << threads;
        log_info(oss.str());
#else
        log_info("mode=parallel (OpenMP not available at runtime)");
#endif
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
        assignment3_task2::init_A(A, N);
        assignment3_task2::init_B(B, N);
        C.resize(N * N);
    }
    catch (const std::bad_alloc&)
    {
        log_error("memory allocation failed");
        return 1;
    }

    const double t0 = now_seconds();

    if (parallel)
    {
        assignment3_task2::multiply_parallel(A, B, C, N);
    }
    else
    {
        assignment3_task2::multiply_serial(A, B, C, N);
    }

    const double t1 = now_seconds();
    const double elapsed_s = (t1 > t0) ? (t1 - t0) : 0.0;
    const double elapsed_ms = elapsed_s * 1000.0;

    // Print corner elements to verify computation (sanity check).
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

    // Compute GFLOPS: matrix multiply is 2*N^3 floating-point operations.
    const double flops =
        2.0 *
        static_cast<double>(N) *
        static_cast<double>(N) *
        static_cast<double>(N);

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
        oss << " flops=" << flops;
        oss << " gflops=" << gflops;
        log_info(oss.str());
    }

    log_info("assignment3-task2 done");
    return 0;
}
