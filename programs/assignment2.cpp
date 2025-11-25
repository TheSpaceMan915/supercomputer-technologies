/*
  assignment2.cpp — Standalone matrix multiply (C++98), no external deps.

  Build:
    # GCC / Clang
    g++ -std=c++98 -O2 -Wall -Wextra -o assignment2 assignment2.cpp

    # MSVC (x64 Native Tools)
    cl /EHsc /W4 assignment2.cpp /Fe:assignment2.exe

  Run (example):
    ./assignment2 512

  CLI:
    assignment2 <N>
    assignment2 --self-test   # optional tiny check; prints PASS/FAIL and exits 0/1

  Notes:
    - Classic triple-loop C = A · B (double precision), row-major contiguous storage.
    - Init: A[i][j] = i + 1,  B[i][j] = 1.0 / (j + 1),  0 ≤ i,j < N.
    - Logs to stdout; errors and usage go to stderr.
*/

#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <new>

/* ----- Logging ----- */

static void log_info(const std::string& m)  { std::cout << "[INFO] "  << m << std::endl; }
static void log_error(const std::string& m) { std::cerr << "[ERROR] " << m << std::endl; }

/* Print concise one-line usage (to stderr). */
static void print_usage() {
  std::cerr << "Usage: assignment2 <N>" << std::endl;
}

/* ----- Parsing ----- */

/* Parse strictly positive integer into out. C++98-friendly. */
static bool parse_positive_int(const char* s, int& out) {
  if (!s || *s == '\0') return false;
  errno = 0;
  char* endp = 0;
  long v = std::strtol(s, &endp, 10);
  if (errno == ERANGE) return false;
  if (endp == s || *endp != '\0') return false;
  if (v <= 0 || v > INT_MAX) return false;
  out = static_cast<int>(v);
  return true;
}

/* ----- Pure computation helpers (no I/O) ----- */

/* Row-major index: i*N + j, returned as size_type for vector access. */
static std::vector<double>::size_type idx(int i, int j, int N) {
  return static_cast<std::vector<double>::size_type>(i) *
         static_cast<std::vector<double>::size_type>(N) +
         static_cast<std::vector<double>::size_type>(j);
}

/* Initialize A[i][j] = i + 1. */
static void initA(std::vector<double>& A, int N) {
  for (int i = 0; i < N; ++i) {
    const double v = static_cast<double>(i + 1);
    for (int j = 0; j < N; ++j) {
      A[idx(i, j, N)] = v;
    }
  }
}

/* Initialize B[i][j] = 1.0 / (j + 1). */
static void initB(std::vector<double>& B, int N) {
  for (int j = 0; j < N; ++j) {
    const double v = 1.0 / static_cast<double>(j + 1);
    for (int i = 0; i < N; ++i) {
      B[idx(i, j, N)] = v;
    }
  }
}

/* C = A · B (classic triple loop). */
static void multiply(const std::vector<double>& A,
                     const std::vector<double>& B,
                     std::vector<double>&       C,
                     int N) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      double acc = 0.0;
      for (int k = 0; k < N; ++k) {
        acc += A[idx(i, k, N)] * B[idx(k, j, N)];
      }
      C[idx(i, j, N)] = acc;
    }
  }
}

/* ----- Optional self-test (no framework) ----- */
/* For N=3, closed-form: C[i][j] = N * (i + 1) / (j + 1).
   We check four boundary elements with tight epsilon. */
static int self_test() {
  const int N = 3;
  std::vector<double> A(static_cast<std::vector<double>::size_type>(N)*N);
  std::vector<double> B(static_cast<std::vector<double>::size_type>(N)*N);
  std::vector<double> C(static_cast<std::vector<double>::size_type>(N)*N);
  initA(A, N);
  initB(B, N);
  multiply(A, B, C, N);

  const double eps = 1e-12;
  const double c00  = C[idx(0,     0,     N)];
  const double c0n1 = C[idx(0,     N - 1, N)];
  const double cn10 = C[idx(N - 1, 0,     N)];
  const double cnn  = C[idx(N - 1, N - 1, N)];

  const double e00  = static_cast<double>(N * 1 / 1);  /* = 3 */
  const double e0n1 = static_cast<double>(N * 1 / N);  /* = 1 */
  const double en10 = static_cast<double>(N * N / 1);  /* = 9 */
  const double enn  = static_cast<double>(N * N / N);  /* = 3 */

  const double d00  = (c00  > e00 ) ? (c00  - e00 ) : (e00  - c00 );
  const double d0n1 = (c0n1 > e0n1) ? (c0n1 - e0n1) : (e0n1 - c0n1);
  const double dn10 = (cn10 > en10) ? (cn10 - en10) : (en10 - cn10);
  const double dnn  = (cnn  > enn ) ? (cnn  - enn ) : (enn  - cnn );

  const bool pass = (d00 <= eps) && (d0n1 <= eps) && (dn10 <= eps) && (dnn <= eps);
  if (pass) {
    std::cout << "PASS" << std::endl;
    return 0;
  } else {
    std::cout << "FAIL" << std::endl;
    return 1;
  }
}

/* ----- Main ----- */

int main(int argc, char** argv) {
  /* Self-test path */
  if (argc == 2 && std::string(argv[1]) == "--self-test") {
    return self_test();
  }

  if (argc != 2) {
    log_error("invalid arguments");
    print_usage();
    return 1;
  }

  int N = 0;
  if (!parse_positive_int(argv[1], N)) {
    std::ostringstream oss; oss << "invalid N: \"" << argv[1] << "\"";
    log_error(oss.str());
    print_usage();
    return 1;
  }

  /* Memory guard: estimate 3*N*N*sizeof(double) and reject if > ~1 GiB. */
  const unsigned long long bytes =
      3ULL *
      static_cast<unsigned long long>(N) *
      static_cast<unsigned long long>(N) *
      static_cast<unsigned long long>(sizeof(double));
  const unsigned long long ONE_GIB = 1ULL << 30;
  if (bytes > ONE_GIB) {
    std::ostringstream oss;
    oss << "allocation would exceed ~1 GiB (estimate=" << bytes << " bytes). Choose smaller N.";
    log_error(oss.str());
    return 1;
  }

  log_info("assignment2 start");
  { std::ostringstream o; o << "N=" << N; log_info(o.str()); }

  try {
    const std::vector<double>::size_type NN =
        static_cast<std::vector<double>::size_type>(N) *
        static_cast<std::vector<double>::size_type>(N);

    std::vector<double> A(NN), B(NN), C(NN);
    initA(A, N);
    initB(B, N);

    const std::clock_t t0 = std::clock();
    multiply(A, B, C, N);
    const std::clock_t t1 = std::clock();

    /* Boundary elements */
    const double c00  = C[idx(0,     0,     N)];
    const double c0n1 = C[idx(0,     N - 1, N)];
    const double cn10 = C[idx(N - 1, 0,     N)];
    const double cnn  = C[idx(N - 1, N - 1, N)];
    {
      std::ostringstream oss;
      oss.setf(std::ios::fixed); oss.precision(12);
      oss << "C[0][0]="    << c00
          << ", C[0][N-1]=" << c0n1
          << ", C[N-1][0]=" << cn10
          << ", C[N-1][N-1]=" << cnn;
      log_info(oss.str());
    }

    /* Timing & performance (CPU time) */
    const double elapsed_ms =
        1000.0 * static_cast<double>(t1 - t0) / static_cast<double>(CLOCKS_PER_SEC);
    const double flops = 2.0 *
        static_cast<double>(N) * static_cast<double>(N) * static_cast<double>(N);
    const double elapsed_s = (elapsed_ms > 0.0) ? (elapsed_ms / 1000.0) : 0.0;
    const double gflops = (elapsed_s > 0.0) ? (flops / (elapsed_s * 1e9)) : 0.0;

    {
      std::ostringstream out;
      out << std::setiosflags(std::ios::fixed) << std::setprecision(2)
          << "elapsed_ms=" << elapsed_ms;
      {
        std::ostringstream fl; fl.setf(std::ios::scientific); fl.precision(6); fl << flops;
        out << " flops=" << fl.str();
      }
      {
        std::ostringstream gf; gf.setf(std::ios::fixed); gf.precision(3); gf << gflops;
        out << " gflops=" << gf.str();
      }
      log_info(out.str());
    }

    log_info("assignment2 done");
    return 0;
  }
  catch (const std::bad_alloc&) {
    log_error("allocation failed: std::bad_alloc");
    return 1;
  }
  catch (const std::exception& e) {
    std::ostringstream oss; oss << "runtime error: " << e.what();
    log_error(oss.str());
    return 1;
  }
}
