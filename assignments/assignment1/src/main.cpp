// main.cpp - Entry point for π approximation CLI
// Parses a single integer n from argv, invokes approximate_pi(n), logs timing/error.
// Lifecycle: start → parse n → compute π → report results (value, error, time) → done.
// Cross-platform: defines _USE_MATH_DEFINES for Windows before including <cmath> to get M_PI.

// Windows requires _USE_MATH_DEFINES before <cmath> to expose M_PI
#ifdef _WIN32
#  ifndef _USE_MATH_DEFINES
#    define _USE_MATH_DEFINES
#  endif
#endif

#include <cmath>
// Fallback M_PI definition if not provided by <cmath> (some platforms/standards)
#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

#include <cstdlib>
#include <cerrno>
#include <climits>
#include <ctime>
#include <sstream>
#include <string>
#include "assignment1/pi.h"
#include "assignment1/logger.h"

using assignment1::approximate_pi;
using assignment1::log_error;
using assignment1::log_info;

// Parse a positive integer from a C-string, returning false on failure.
// Validates: non-empty, base-10, no overflow/underflow, 1..INT_MAX range.
// On success, writes result to 'out' and returns true.
static bool parse_positive_int(const char* s, int& out)
{
    if (!s || *s == '\0') return false;
    errno = 0;
    char* endp = 0;
    long v = std::strtol(s, &endp, 10);
    if (errno == ERANGE) return false;           // Overflow/underflow
    if (endp == s || *endp != '\0') return false; // No digits or trailing junk
    if (v <= 0 || v > INT_MAX) return false;      // Out of valid range
    out = static_cast<int>(v);
    return true;
}

int main(int argc, char** argv)
{
    log_info("assignment1 start");

    // Require exactly one command-line argument: n (number of subintervals)
    if (argc != 2) {
        log_error("Usage: assignment1 <n>  (n must be a positive integer)");
        return 1;
    }

    int n = 0;
    if (!parse_positive_int(argv[1], n)) {
        log_error("Invalid n. Please provide a positive integer within range.");
        return 1;
    }

    // Sanity check: cap n at 1e9 to avoid excessively long runs
    const int MAX_N = 1000000000;
    if (n > MAX_N) {
        log_error("n is too large for a reasonable run. Try <= 1e9.");
        return 1;
    }

    {
        std::ostringstream oss;
        oss << "Parsed n = " << n;
        log_info(oss.str());
    }

    // Time the π approximation
    const std::clock_t t0 = std::clock();
    const double pi_est = approximate_pi(n);
    const std::clock_t t1 = std::clock();

    // Compute absolute error vs. reference M_PI
    const double abs_err = (pi_est > M_PI) ? (pi_est - M_PI) : (M_PI - pi_est);
    const double secs = static_cast<double>(t1 - t0) / static_cast<double>(CLOCKS_PER_SEC);

    // Report π estimate with high precision
    {
        std::ostringstream oss;
        oss.setf(std::ios::fixed);
        oss.precision(12);
        oss << "pi_est = " << pi_est;
        log_info(oss.str());
    }
    // Report absolute error
    {
        std::ostringstream oss;
        oss.setf(std::ios::scientific);
        oss.precision(12);
        oss << "abs error vs M_PI = " << abs_err;
        log_info(oss.str());
    }
    // Report elapsed time in seconds
    {
        std::ostringstream oss;
        oss.setf(std::ios::fixed);
        oss.precision(3);
        oss << "elapsed = " << secs << " s";
        log_info(oss.str());
    }

    log_info("assignment1 done");
    return 0;
}
