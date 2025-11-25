/*
  assignment1.cpp — standalone C++98 program (no deps)

  What it does
  ------------
  Approximates π using the midpoint Riemann sum:
      pi ≈ (1/n) * Σ_{i=1..n} 4 / (1 + ((i - 0.5)/n)^2)
  Logs the workflow, prints the approximation, absolute error vs M_PI,
  and elapsed time in milliseconds. Also supports `--self-test`.

  Build (Linux/macOS, GCC/Clang)
  ------------------------------
  g++ -std=c++98 -O2 -Wall -Wextra -o assignment1 assignment1.cpp

  Build (Windows, MSVC x64 Native Tools)
  --------------------------------------
  cl /EHsc /W4 assignment1.cpp /Fe:assignment1.exe

  Example run
  -----------
  ./assignment1 100
  [INFO] start
  [INFO] n=10
  [INFO] pi=3.14159265 error=8.3333e-06 elapsed_ms=XX
  [INFO] done
*/

#ifdef _WIN32
#  ifndef _USE_MATH_DEFINES
#    define _USE_MATH_DEFINES
#  endif
#endif

#include <cmath>
#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

#include <cstdlib>
#include <cerrno>
#include <climits>
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

static void log_info(const std::string& msg)  { std::cout << "[INFO] "  << msg << std::endl; }
static void log_error(const std::string& msg) { std::cout << "[ERROR] " << msg << std::endl; }

static void print_usage_line()
{
    std::cerr << "Usage: assignment1 <n>  (positive integer)  |  assignment1 --self-test" << std::endl;
}

static double approximate_pi(int n)
{
    if (n <= 0) return 0.0;

    const double inv_n = 1.0 / static_cast<double>(n);
    double sum = 0.0;

    for (int i = 1; i <= n; ++i) {
        const double x = (static_cast<double>(i) - 0.5) * inv_n;
        sum += 4.0 / (1.0 + x * x);
    }
    return sum * inv_n;
}

static bool parse_positive_int(const char* s, int& out)
{
    if (!s || *s == '\0') return false;
    errno = 0;
    char* endp = 0;
    const long v = std::strtol(s, &endp, 10);
    if (errno == ERANGE) return false;
    if (endp == s || *endp != '\0') return false;
    if (v <= 0 || v > INT_MAX) return false;
    out = static_cast<int>(v);
    return true;
}

static int run_self_test()
{
    const int n = 100000;
    const double pi_est = approximate_pi(n);
    const double err = (pi_est > M_PI) ? (pi_est - M_PI) : (M_PI - pi_est);
    if (err < 1e-5) {
        log_info("self-test PASS: n=100000, error<1e-5");
        return 0;
    } else {
        std::ostringstream oss;
        oss.setf(std::ios::scientific);
        oss.precision(6);
        oss << "self-test FAIL: error=" << err << " (expected < 1e-5)";
        log_error(oss.str());
        return 1;
    }
}

int main(int argc, char** argv)
{
    if (argc == 2 && std::string(argv[1]) == "--self-test") {
        return run_self_test();
    }

    log_info("start");

    if (argc != 2) {
        log_error("invalid arguments");
        print_usage_line();
        return 1;
    }

    int n = 0;
    if (!parse_positive_int(argv[1], n)) {
        std::ostringstream oss;
        oss << "invalid n: \"" << argv[1] << "\"";
        log_error(oss.str());
        print_usage_line();
        return 1;
    }

    const int MAX_N = 1000000000; /* 1e9 */
    if (n > MAX_N) {
        std::ostringstream oss;
        oss << "n too large (" << n << "). Please use n <= " << MAX_N << ".";
        log_error(oss.str());
        print_usage_line();
        return 1;
    }

    {
        std::ostringstream oss;
        oss << "n=" << n;
        log_info(oss.str());
    }

    const std::clock_t t0 = std::clock();
    const double pi_est = approximate_pi(n);
    const std::clock_t t1 = std::clock();

    const double abs_err = (pi_est > M_PI) ? (pi_est - M_PI) : (M_PI - pi_est);
    const long elapsed_ms = static_cast<long>(
        (static_cast<double>(t1 - t0) * 1000.0) / static_cast<double>(CLOCKS_PER_SEC));

    {
        std::ostringstream oss;
        oss.setf(std::ios::fixed);
        oss.precision(8);
        oss << "pi=" << pi_est;

        std::ostringstream ose;
        ose.setf(std::ios::scientific);
        ose.precision(6);
        ose << abs_err;

        std::ostringstream out;
        out << oss.str()
            << " error=" << ose.str()
            << " elapsed_ms=" << elapsed_ms;
        log_info(out.str());
    }

    log_info("done");
    return 0;
}
