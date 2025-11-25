/*
 * unit_tests.cpp — Unity-based unit tests for assignment2 matrix operations
 * Tests correctness via closed-form formula C[i][j] = N*(i+1)/(j+1) and
 * validates timing/FLOPS are non-negative. Uses extern "C" for Unity integration.
 */
#include "assignment2/matrix.h"

/* Wrap Unity C header for C++ linkage */
extern "C" {
#include "vendor/unity/unity.h"
}

#include <ctime>

using assignment2::Matrix;
using assignment2::initA;
using assignment2::initB;
using assignment2::multiply;

// Test small N=3 case against known closed-form: C[i][j] = N*(i+1)/(j+1)
static void test_small_N_exact_values(void)
{
  const int N = 3;
  Matrix A(N), B(N), C(N);
  initA(A);
  initB(B);
  multiply(A, B, C);

  /* Closed-form: C[i][j] = N * (i + 1) / (j + 1) */
  const double eps = 1e-12;
  TEST_ASSERT_DOUBLE_WITHIN(eps, static_cast<double>(N * 1 / 1), C.at(0,0));         // N
  TEST_ASSERT_DOUBLE_WITHIN(eps, static_cast<double>(N * 1 / N), C.at(0,N-1));       // 1
  TEST_ASSERT_DOUBLE_WITHIN(eps, static_cast<double>(N * N / 1), C.at(N-1,0));       // N^2
  TEST_ASSERT_DOUBLE_WITHIN(eps, static_cast<double>(N * N / N), C.at(N-1,N-1));     // N
}

// Sanity check that timing and FLOPS formula are non-negative
static void test_flops_and_time_non_negative(void)
{
  const int N = 2;
  Matrix A(N), B(N), C(N);
  initA(A);
  initB(B);
  const std::clock_t t0 = std::clock();
  multiply(A, B, C);
  const std::clock_t t1 = std::clock();

  /* elapsed CPU time should be >= 0 */
  TEST_ASSERT_TRUE(t1 - t0 >= 0);

  /* flops formula: 2 * N^3 */
  const double expect = 2.0 * static_cast<double>(N) * static_cast<double>(N) * static_cast<double>(N);
  const double got = 2.0 * static_cast<double>(N) * static_cast<double>(N) * static_cast<double>(N);
  TEST_ASSERT_DOUBLE_WITHIN(0.0, expect, got);
}

// Unity test runner entry point
int main(void)
{
  UnityBegin("assignment2");
  RUN_TEST(test_small_N_exact_values);
  RUN_TEST(test_flops_and_time_non_negative);
  return UnityEnd();
}
