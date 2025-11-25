/*
 * main.cpp — CLI driver for assignment2 matrix multiplication benchmark
 * Parses N from argv, initializes 3 NxN matrices, runs C = A·B, reports corner
 * values, timing (CPU via std::clock()), and GFLOPS. Guards large allocations.
 */
#include "assignment2/matrix.h"
#include "assignment2/logger.h"

#include <cstdlib>
#include <cerrno>
#include <climits>
#include <ctime>
#include <sstream>
#include <string>
#include <new>
#include <iostream>

using assignment2::Matrix;
using assignment2::initA;
using assignment2::initB;
using assignment2::multiply;
using assignment2::log_error;
using assignment2::log_info;

static void usage(){ std::cerr << "Usage: assignment2 <N>" << std::endl; }

// Parse positive integer from C-string; returns false on error or out-of-range
static bool parse_positive_int(const char* s, int& out){
  if (!s || *s == '\0') return false;
  errno = 0; char* endp = 0; long v = std::strtol(s, &endp, 10);
  if (errno == ERANGE) return false;
  if (endp == s || *endp != '\0') return false;
  if (v <= 0 || v > INT_MAX) return false;
  out = static_cast<int>(v); return true;
}

int main(int argc, char** argv){
  if (argc != 2){ log_error("invalid arguments"); usage(); return 1; }
  int N = 0; if (!parse_positive_int(argv[1], N)){ std::ostringstream oss; oss << "invalid N: \"" << argv[1] << "\""; log_error(oss.str()); usage(); return 1; }

  // Check if 3 NxN matrices would exceed ~1 GiB to prevent huge allocations
  const unsigned long long bytes = 3ULL * (unsigned long long)N * (unsigned long long)N * (unsigned long long)sizeof(double);
  const unsigned long long ONE_GIB = 1ULL << 30;
  if (bytes > ONE_GIB){ std::ostringstream oss; oss << "allocation would exceed ~1 GiB (estimate=" << bytes << " bytes). Choose smaller N."; log_error(oss.str()); return 1; }

  log_info("assignment2 start"); { std::ostringstream o; o << "N=" << N; log_info(o.str()); }

  try{
    Matrix A(N), B(N), C(N);
    initA(A); initB(B);

    // Time the multiplication using CPU clock ticks
    const std::clock_t t0 = std::clock();
    multiply(A, B, C);
    const std::clock_t t1 = std::clock();

    // Report corner values for correctness checking
    const double c00  = C.at(0,0);
    const double c0n1 = C.at(0,N-1);
    const double cn10 = C.at(N-1,0);
    const double cnn  = C.at(N-1,N-1);

    { std::ostringstream oss; oss.setf(std::ios::fixed); oss.precision(12);
      oss << "C[0][0]=" << c00
          << ", C[0][N-1]=" << c0n1
          << ", C[N-1][0]=" << cn10
          << ", C[N-1][N-1]=" << cnn;
      log_info(oss.str()); }

    // Compute GFLOPS: 2*N^3 FLOPs for matmul, convert clock ticks to seconds
    const double elapsed_ms = 1000.0 * (double)(t1 - t0) / (double)CLOCKS_PER_SEC;
    const double flops = 2.0 * (double)N * (double)N * (double)N;
    const double elapsed_s = (elapsed_ms > 0.0) ? (elapsed_ms / 1000.0) : 0.0;
    const double gflops = (elapsed_s > 0.0) ? (flops / (elapsed_s * 1e9)) : 0.0;

    { std::ostringstream ms; ms.setf(std::ios::fixed); ms.precision(2); ms << elapsed_ms;
      std::ostringstream fl; fl.setf(std::ios::scientific); fl.precision(3); fl << flops;
      std::ostringstream gf; gf.setf(std::ios::fixed); gf.precision(3); gf << gflops;
      std::ostringstream out; out << "elapsed_ms=" << ms.str() << " flops=" << fl.str() << " gflops=" << gf.str();
      log_info(out.str()); }

    log_info("assignment2 done");
    return 0;
  } catch(const std::bad_alloc&){ log_error("allocation failed: std::bad_alloc"); return 1; }
    catch(const std::exception& e){ std::ostringstream oss; oss << "runtime error: " << e.what(); log_error(oss.str()); return 1; }
}
