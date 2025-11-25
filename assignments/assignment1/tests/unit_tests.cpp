// unit_tests.cpp - Unity-based unit tests for π approximation
// Unity is a C framework; we wrap its header in extern "C" for C++ linkage.
// Tests verify that approximate_pi(n) converges to M_PI within acceptable error.

#include "assignment1/pi.h"

// Unity is compiled as C; include its header with C linkage to avoid mangling
extern "C" {
#include "vendor/unity/unity.h"
}

// Cross-platform M_PI setup (same approach as main.cpp)
#ifdef _WIN32
#  ifndef _USE_MATH_DEFINES
#    define _USE_MATH_DEFINES
#  endif
#endif
#include <cmath>
#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

// Test: verify midpoint rule converges to π within expected error bound.
// For n=100000, midpoint rule error should be O(1/n²) ≈ 1e-10, but we use
// a relaxed threshold of 1e-5 to account for floating-point variance.
static void test_midpoint_rule_converges(void)
{
    const int n = 100000;
    const double approx = assignment1::approximate_pi(n);
    const double err = (approx > M_PI) ? (approx - M_PI) : (M_PI - approx);
    TEST_ASSERT_TRUE(err < 1e-5);
}

// Unity test runner: initialize, run all tests, report results
int main(void)
{
    UnityBegin("assignment1");
    RUN_TEST(test_midpoint_rule_converges);
    return UnityEnd();
}
