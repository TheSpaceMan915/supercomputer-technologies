// unit_tests.cpp — Unity-based unit tests for pi approximation functions
// Tests verify accuracy, serial/parallel consistency, and convergence behavior.

#include "assignment3_task1/pi.h"

extern "C" {
#include "vendor/unity/unity.h"
}

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

// Test: parallel implementation achieves reasonable accuracy with modest n
// Pass: error < 1e-4 for n=1000 (midpoint rule converges as O(1/n²))
static void test_parallel_accuracy_small_n(void) {
  const int n = 1000;
  const double computed_pi = assignment3_task1::approximate_pi_parallel(n);
  const double error = (computed_pi > M_PI) ? (computed_pi - M_PI) : (M_PI - computed_pi);
  TEST_ASSERT_TRUE(error < 1e-4);
}

// Test: serial and parallel implementations produce identical results
// Pass: results match within floating-point tolerance (1e-12)
static void test_serial_parallel_match(void) {
  const int n = 10000;
  const double serial_result = assignment3_task1::approximate_pi_serial(n);
  const double parallel_result = assignment3_task1::approximate_pi_parallel(n);
  TEST_ASSERT_DOUBLE_WITHIN(1e-12, serial_result, parallel_result);
}

// Test: error decreases (or stays equal) as n increases
// Pass: error(n=4000) <= error(n=2000), confirming convergence
static void test_error_monotonicity(void) {
  const int n1 = 2000;
  const int n2 = 4000;
  const double pi1 = assignment3_task1::approximate_pi_parallel(n1);
  const double pi2 = assignment3_task1::approximate_pi_parallel(n2);
  const double error1 = (pi1 > M_PI) ? (pi1 - M_PI) : (M_PI - pi1);
  const double error2 = (pi2 > M_PI) ? (pi2 - M_PI) : (M_PI - pi2);
  TEST_ASSERT_TRUE(error2 <= error1);
}

int main(void) {
  UnityBegin("assignment3-task1");
  RUN_TEST(test_parallel_accuracy_small_n);
  RUN_TEST(test_serial_parallel_match);
  RUN_TEST(test_error_monotonicity);
  return UnityEnd();
}
