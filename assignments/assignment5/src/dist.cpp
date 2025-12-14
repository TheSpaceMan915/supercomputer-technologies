#include "assignment5/dist.h"

namespace a5 {

void row_block_partition(int N, int P, int rank, int& offset, int& count) {
  if (P <= 0) { offset = 0; count = 0; return; }
  const int base = N / P;
  const int rem  = N % P;
  if (rank < rem) { offset = rank * (base + 1); count = base + 1; }
  else { offset = rem * (base + 1) + (rank - rem) * base; count = base; }
}

int owner_of_row(int N, int P, int row) {
  if (row < 0 || row >= N || P <= 0) return 0;
  const int base = N / P;
  const int rem  = N % P;
  if (row < (base + 1) * rem) return row / (base + 1);
  const int after = row - (base + 1) * rem;
  return rem + after / base;
}

} // namespace a5
