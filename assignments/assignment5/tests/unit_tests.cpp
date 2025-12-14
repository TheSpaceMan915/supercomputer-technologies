/**
 * @file unit_tests.cpp
 * @brief Unit tests for assignment5 distribution and matrix functions.
 *
 * Tests the row-block partitioning logic to ensure correct distribution
 * of rows across MPI ranks. Uses the Unity test framework.
 */

#include "assignment5/dist.h"
#include "assignment5/matrix.h"
extern "C" {
#include "vendor/unity/unity.h"
}
#include <vector>

/**
 * @brief Test basic row-block partitioning with N=10, P=3.
 *
 * Verifies that rows are distributed as:
 *   rank 0: rows [0, 3]   (4 rows)
 *   rank 1: rows [4, 6]   (3 rows)
 *   rank 2: rows [7, 9]   (3 rows)
 */
static void test_row_block_partition_basic() {
  int off = 0, cnt = 0;
  
  a5::row_block_partition(10, 3, 0, off, cnt);
  UnityAssertEqualInt(0, off, "rank0 off");
  UnityAssertEqualInt(4, cnt, "rank0 cnt");
  
  a5::row_block_partition(10, 3, 1, off, cnt);
  UnityAssertEqualInt(4, off, "rank1 off");
  UnityAssertEqualInt(3, cnt, "rank1 cnt");
  
  a5::row_block_partition(10, 3, 2, off, cnt);
  UnityAssertEqualInt(7, off, "rank2 off");
  UnityAssertEqualInt(3, cnt, "rank2 cnt");
}

/**
 * @brief Test row ownership determination.
 *
 * Ensures that owner_of_row() correctly identifies which rank owns
 * each row given the same N=10, P=3 partitioning.
 */
static void test_owner_of_row() {
  UnityAssertEqualInt(0, a5::owner_of_row(10, 3, 0), "row0 owner");
  UnityAssertEqualInt(0, a5::owner_of_row(10, 3, 3), "row3 owner");
  UnityAssertEqualInt(1, a5::owner_of_row(10, 3, 4), "row4 owner");
  UnityAssertEqualInt(2, a5::owner_of_row(10, 3, 7), "row7 owner");
  UnityAssertEqualInt(2, a5::owner_of_row(10, 3, 9), "row9 owner");
}

int main() {
  UnityBegin("assignment5");
  RUN_TEST(test_row_block_partition_basic, "row_block_partition_basic");
  RUN_TEST(test_owner_of_row, "owner_of_row");
  UnityEnd();
  return 0;
}
