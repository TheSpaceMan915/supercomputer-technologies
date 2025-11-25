// assignment3_task1.cpp
//
// Build (Linux/macOS, GCC/Clang with OpenMP):
//   g++ -std=c++98 -O2 -fopenmp -Wall -Wextra -o assignment3_task1 assignment3_task1.cpp
//
// Build (Linux/macOS without OpenMP):
//   g++ -std=c++98 -O2 -Wall -Wextra -o assignment3_task1 assignment3_task1.cpp
//
// Build (Windows, MSVC, with OpenMP):
//   cl /EHsc /W4 /openmp assignment3_task1.cpp /Fe:assignment3_task1.exe
//
// Run:
//   ./assignment3_task1 1000000
//
// This program approximates pi using the midpoint rule:
//   pi ≈ (1/n) * sum_{i=1..n} 4 / (1 + ((i - 0.5)/n)^2)
// It uses OpenMP (if available) to parallelize the summation.
// On invalid input, prints an error + usage and exits non-zero.

#ifdef _MSC_VER
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#endif

#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <climits>
#include <ctime>

#ifdef _OPENMP
#include <omp.h>
#endif

// Forward declarations

// Parse and validate n from argv.
// On error, prints [ERROR] and usage, returns false.
bool parse_n(int argc, char** argv, int& n);

// Serial midpoint implementation (always available).
double approximate_pi_serial(int n);

// Parallel midpoint implementation.
// Uses OpenMP reduction when _OPENMP is defined,
// otherwise falls back to approximate_pi_serial.
double approximate_pi_parallel(int n);

// Current time in milliseconds.
// Uses omp_get_wtime() when OpenMP is enabled, otherwise std::clock().
double now_ms();

// Simple logging helpers.
void log_info(const char* msg);
void log_error(const char* msg);
void log_info_stream(const std::string& msg);

// Get number of threads used (or 1 if OpenMP is not enabled).
int get_thread_count();

// Implementation

bool parse_n(int argc, char** argv, int& n)
{
    if (argc != 2)
    {
        std::fprintf(stderr, "[ERROR] invalid arguments\n");
        std::fprintf(stderr, "Usage: assignment3_task1 <n>\n");
        return false;
    }

    const char* s = argv[1];
    if (!s || *s == '\0')
    {
        std::fprintf(stderr, "[ERROR] missing n\n");
        std::fprintf(stderr, "Usage: assignment3_task1 <n>\n");
        return false;
    }

    errno = 0;
    char* endp = 0;
    long val = std::strtol(s, &endp, 10);

    if (errno == ERANGE)
    {
        std::fprintf(stderr, "[ERROR] n out of range: %s\n", s);
        std::fprintf(stderr, "Usage: assignment3_task1 <n>\n");
        return false;
    }

    if (endp == s || *endp != '\0')
    {
        std::fprintf(stderr, "[ERROR] invalid n (not an integer): %s\n", s);
        std::fprintf(stderr, "Usage: assignment3_task1 <n>\n");
        return false;
    }

    if (val <= 0)
    {
        std::fprintf(stderr, "[ERROR] n must be positive: %ld\n", val);
        std::fprintf(stderr, "Usage: assignment3_task1 <n>\n");
        return false;
    }

    // Guard against unreasonably large n (loop cost / overflow risk).
    // This does not need to be exact; just a conservative safety check.
    if (val > 2000000000L)
    {
        std::fprintf(stderr, "[ERROR] n is unreasonably large (>%ld)\n", 2000000000L);
        std::fprintf(stderr, "Usage: assignment3_task1 <n>\n");
        return false;
    }

    n = static_cast<int>(val);
    return true;
}

double approximate_pi_serial(int n)
{
    if (n <= 0)
    {
        return 0.0;
    }

    const double w = 1.0 / static_cast<double>(n);
    double sum = 0.0;

    // i = 0..n-1, x = (i + 0.5) * w
    for (int i = 0; i < n; ++i)
    {
        const double x = (static_cast<double>(i) + 0.5) * w;
        sum += 4.0 / (1.0 + x * x);
    }

    return w * sum;
}

double approximate_pi_parallel(int n)
{
    if (n <= 0)
    {
        return 0.0;
    }

#ifndef _OPENMP
    // Compiled without OpenMP support: use the serial implementation.
    return approximate_pi_serial(n);
#else
    const double w = 1.0 / static_cast<double>(n);
    double sum = 0.0;

    // Classic OpenMP 3.0 parallel loop with reduction.
    #pragma omp parallel for reduction(+:sum) schedule(static)
    for (int i = 0; i < n; ++i)
    {
        const double x = (static_cast<double>(i) + 0.5) * w;
        sum += 4.0 / (1.0 + x * x);
    }

    return w * sum;
#endif
}

double now_ms()
{
#ifdef _OPENMP
    // Wall-clock time in seconds; convert to ms.
    return omp_get_wtime() * 1000.0;
#else
    // CPU time; may differ from wall-clock but is portable in C++98.
    const std::clock_t c = std::clock();
    return 1000.0 * static_cast<double>(c) / static_cast<double>(CLOCKS_PER_SEC);
#endif
}

void log_info(const char* msg)
{
    std::cout << "[INFO] " << msg << std::endl;
}

void log_error(const char* msg)
{
    std::cerr << "[ERROR] " << msg << std::endl;
}

void log_info_stream(const std::string& msg)
{
    std::cout << "[INFO] " << msg << std::endl;
}

int get_thread_count()
{
#ifdef _OPENMP
    // Number of threads that OpenMP may use.
    int t = omp_get_max_threads();
    if (t <= 0)
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
    int n = 0;
    if (!parse_n(argc, argv, n))
    {
        return 1; // parse_n already printed error + usage
    }

    log_info("assignment3-task1 start");

    {
        std::ostringstream oss;
        oss << "n=" << n;
        log_info_stream(oss.str());
    }

    {
        std::ostringstream oss;
        oss << "threads=" << get_thread_count();
        log_info_stream(oss.str());
    }

    const double t0 = now_ms();
    const double pi = approximate_pi_parallel(n);
    const double t1 = now_ms();

    const double diff = (pi >= M_PI) ? (pi - M_PI) : (M_PI - pi);
    const double elapsed_ms = t1 - t0;

    {
        std::ostringstream oss;
        oss.setf(std::ios::fixed);
        oss << std::setprecision(8);
        oss << "pi=" << pi;

        std::ostringstream eoss;
        eoss.setf(std::ios::scientific);
        eoss << std::setprecision(6) << diff;

        oss << " error=" << eoss.str()
            << " elapsed_ms=" << std::setprecision(2) << std::fixed
            << (elapsed_ms >= 0.0 ? elapsed_ms : 0.0);

        log_info_stream(oss.str());
    }

    log_info("assignment3-task1 done");
    return 0;
}
