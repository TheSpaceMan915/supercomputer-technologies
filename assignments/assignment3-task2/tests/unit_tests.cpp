// unit_tests.cpp: Unity-based tests for assignment3-task2 matrix operations.
// Validates correctness of initialization and serial/parallel multiplication.
#include "assignment3_task2/matrix.h"

extern "C" {
#include "vendor/unity/unity.h"
}

#include <vector>

// Test small 2×2 matrix multiplication for known values.
// A = [[1,1], [2,2]], B = [[1,0.5], [1,0.5]] => C = [[2,1], [4,2]]
static void test_small_N_2(void)
{
    const int N = 2;
    std::vector<double> A;
    std::vector<double> B;
    std::vector<double> C;

    assignment3_task2::init_A(A, N);
    assignment3_task2::init_B(B, N);
    assignment3_task2::multiply_serial(A, B, C, N);

    TEST_ASSERT_TRUE(C.size() == 4);
    TEST_ASSERT_DOUBLE_WITHIN(1e-12, 2.0, C[0]);
    TEST_ASSERT_DOUBLE_WITHIN(1e-12, 1.0, C[1]);
    TEST_ASSERT_DOUBLE_WITHIN(1e-12, 4.0, C[2]);
    TEST_ASSERT_DOUBLE_WITHIN(1e-12, 2.0, C[3]);
}

// Verify parallel implementation produces identical results to serial.
// Tests 3×3 case; deterministic result regardless of thread count.
static void test_parallel_matches_serial_3(void)
{
    const int N = 3;
    std::vector<double> A;
    std::vector<double> B;
    std::vector<double> Cs;
    std::vector<double> Cp;

    assignment3_task2::init_A(A, N);
    assignment3_task2::init_B(B, N);

    assignment3_task2::multiply_serial(A, B, Cs, N);
    assignment3_task2::multiply_parallel(A, B, Cp, N);

    TEST_ASSERT_TRUE(Cs.size() == Cp.size());
    for (size_t i = 0; i < Cs.size(); ++i)
    {
        TEST_ASSERT_DOUBLE_WITHIN(1e-12, Cs[i], Cp[i]);
    }
}

int main(void)
{
    UnityBegin("assignment3-task2");

    RUN_TEST(test_small_N_2);
    RUN_TEST(test_parallel_matches_serial_3);

    return UnityEnd();
}
