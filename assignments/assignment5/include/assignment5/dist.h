/**
 * @file dist.h
 * @brief Row-block distribution utilities for MPI GEMM.
 *
 * Implements a simple block distribution scheme that assigns contiguous
 * rows of an N x N matrix to P MPI ranks. Ranks with smaller indices
 * receive one extra row if N is not evenly divisible by P.
 */

#ifndef ASSIGNMENT5_DIST_H
#define ASSIGNMENT5_DIST_H

namespace a5 {

/**
 * @brief Compute the row range owned by a given rank.
 *
 * Distributes N rows across P ranks using a block partitioning strategy.
 * The first (N % P) ranks receive (N / P + 1) rows each, and the remaining
 * ranks receive (N / P) rows each. This ensures all rows are assigned and
 * load is balanced as evenly as possible.
 *
 * @param N      Total number of rows to distribute
 * @param P      Total number of ranks (processes)
 * @param rank   The rank whose partition to compute (0 <= rank < P)
 * @param offset Output: starting row index for this rank
 * @param count  Output: number of rows owned by this rank
 */
void row_block_partition(int N, int P, int rank, int& offset, int& count);

/**
 * @brief Determine which rank owns a given row.
 *
 * Given a global row index, returns the rank that owns it under the
 * row-block partitioning scheme used by row_block_partition().
 *
 * @param N   Total number of rows
 * @param P   Total number of ranks
 * @param row Global row index (0 <= row < N)
 * @return The rank that owns the specified row (0 if out of bounds)
 */
int owner_of_row(int N, int P, int row);

} // namespace a5

#endif
