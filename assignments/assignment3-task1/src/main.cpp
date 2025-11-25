// main.cpp — Driver program for pi approximation with timing and error reporting
// Parses command-line arguments, runs parallel pi computation, and logs results.
// Requires one argument: the number of intervals (n) for the midpoint rule.

// Ensure M_PI is defined on MSVC
#ifdef _MSC_VER
#  ifndef _USE_MATH_DEFINES
#    define _USE_MATH_DEFINES
#  endif
#endif

#include <cmath>

// Define M_PI if the platform doesn't provide it
#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

#include "assignment3_task1/pi.h"
#include "assignment3_task1/logger.h"

#ifdef _OPENMP
#  include <omp.h>
#endif

#include <cstdlib>
#include <cerrno>
#include <climits>
#include <ctime>
#include <sstream>
#include <string>
#include <iostream>

using assignment3_task1::approximate_pi_parallel;
using assignment3_task1::log_error;
using assignment3_task1::log_info;

// Print usage message to stderr
static void print_usage() {
  std::cerr << "Usage: assignment3-task1 <n>" << std::endl;
}

// Parse a positive integer from a string; returns false on error
static bool parse_positive_int(const char* str, int& result) {
  if (!str || *str == '\0') {
    return false;
  }

  errno = 0;
  char* end_ptr = 0;
  const long value = strtol(str, &end_ptr, 10);

  // Check for conversion errors
  if (errno == ERANGE) {
    return false;
  }
  if (end_ptr == str || *end_ptr != '\0') {
    return false;
  }

  // Ensure value is positive and fits in int
  if (value <= 0 || value > INT_MAX) {
    return false;
  }

  result = static_cast<int>(value);
  return true;
}

// Get current wall-clock time in milliseconds
static double get_wall_time_ms() {
#ifdef _OPENMP
  // Use high-resolution OpenMP timer if available
  return omp_get_wtime() * 1000.0;
#else
  // Fall back to clock() (less precise, may wrap on long runs)
  const clock_t ticks = clock();
  return 1000.0 * static_cast<double>(ticks) / static_cast<double>(CLOCKS_PER_SEC);
#endif
}

int main(int argc, char** argv) {
  // Validate argument count
  if (argc != 2) {
    log_error("invalid arguments");
    print_usage();
    return 1;
  }

  // Parse the number of intervals
  int n = 0;
  if (!parse_positive_int(argv[1], n)) {
    std::ostringstream oss;
    oss << "invalid n: \"" << argv[1] << "\"";
    log_error(oss.str());
    print_usage();
    return 1;
  }

  // Sanity check: reject unreasonably large values to avoid long runtimes
  if (n > 2000000000L) {
    log_error("n is unreasonably large; choose a smaller value (<= 2e9)");
    return 1;
  }

  // Determine thread count (1 if OpenMP not available)
  const int thread_count =
#ifdef _OPENMP
      omp_get_max_threads();
#else
      1;
#endif

  // Log startup info
  log_info("assignment3-task1 start");
  {
    std::ostringstream oss;
    oss << "n=" << n << " threads=" << thread_count;
    log_info(oss.str());
  }

  // Compute pi with timing
  const double start_time = get_wall_time_ms();
  const double computed_pi = approximate_pi_parallel(n);
  const double end_time = get_wall_time_ms();

  // Calculate absolute error
  const double error = (computed_pi > M_PI) ? (computed_pi - M_PI) : (M_PI - computed_pi);
  const long elapsed_ms = static_cast<long>(end_time - start_time + 0.5);

  // Log results with appropriate precision
  {
    std::ostringstream pi_stream;
    pi_stream.setf(std::ios::fixed);
    pi_stream.precision(8);
    pi_stream << "pi=" << computed_pi;

    std::ostringstream error_stream;
    error_stream.setf(std::ios::scientific);
    error_stream.precision(6);
    error_stream << error;

    std::ostringstream output;
    output << pi_stream.str() << " error=" << error_stream.str()
           << " elapsed_ms=" << elapsed_ms;
    log_info(output.str());
  }

  log_info("assignment3-task1 done");
  return 0;
}
