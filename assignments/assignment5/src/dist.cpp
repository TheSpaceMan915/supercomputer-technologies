/**
 * @file dist.cpp
 * @brief Implementation of row-block distribution functions.
 *
 * Provides the logic for distributing rows across MPI ranks in a balanced
 * manner, ensuring each rank receives approximately N / P rows.
 */

#include "assignment5/dist.h"

namespace a5 {

void row_block_partition(int N, int P, int rank, int& offset, int& count) {
  // Handle degenerate case
  if (P <= 0) {
    offset = 0;
    count = 0;
    return;
  }
  
  const int base = N / P;        // Base number of rows per rank
  const int rem = N % P;         // Number of ranks that get one extra row
  
  // Ranks [0, rem) get (base + 1) rows each
  if (rank < rem) {
    offset = rank * (base + 1);
    count = base + 1;
  } else {
    // Ranks [rem, P) get base rows each
    offset = rem * (base + 1) + (rank - rem) * base;
    count = base;
  }
}

int owner_of_row(int N, int P, int row) {
  // Bounds check: return rank 0 for invalid inputs
  if (row < 0 || row >= N || P <= 0) {
    return 0;
  }
  
  const int base = N / P;
  const int rem = N % P;
  
  // First rem ranks own (base + 1) rows each
  if (row < (base + 1) * rem) {
    return row / (base + 1);
  }
  
  // Remaining rows are owned by ranks [rem, P)
  const int after = row - (base + 1) * rem;
  return rem + after / base;
}

} // namespace a5
