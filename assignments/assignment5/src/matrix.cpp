#include "assignment5/matrix.h"
#include <cstddef>

namespace a5 {

void init_B(std::vector<double>& B, int N) {
  B.assign(static_cast<std::size_t>(N) * static_cast<std::size_t>(N), 0.0);
  for (int i = 0; i < N; ++i) {
    const std::size_t row = static_cast<std::size_t>(i) * static_cast<std::size_t>(N);
    for (int j = 0; j < N; ++j) {
      B[row + static_cast<std::size_t>(j)] = 1.0 / static_cast<double>(j + 1);
    }
  }
}

void compute_local_rows(
    int N, int row_offset, int row_count, const std::vector<double>& B,
    double* c00, double* c0N1, double* cN10, double* cN1N1) {
  for (int li = 0; li < row_count; ++li) {
    const int i_glob = row_offset + li;
    for (int j = 0; j < N; ++j) {
      double sum = 0.0;
      for (int k = 0; k < N; ++k) {
        const std::size_t idxB = static_cast<std::size_t>(k) * static_cast<std::size_t>(N)
                                + static_cast<std::size_t>(j);
        sum += static_cast<double>(i_glob + 1) * B[idxB];
      }
      if (i_glob == 0 && j == 0 && c00) *c00 = sum;
      if (i_glob == 0 && j == N-1 && c0N1) *c0N1 = sum;
      if (i_glob == N-1 && j == 0 && cN10) *cN10 = sum;
      if (i_glob == N-1 && j == N-1 && cN1N1) *cN1N1 = sum;
    }
  }
}

bool exceeds_memory_budget_for_B(int N, unsigned long long threshold_bytes) {
  const unsigned long long n = (unsigned long long)N;
  const unsigned long long bytes = 8ULL * n * n;
  return bytes > threshold_bytes;
}

} // namespace a5
