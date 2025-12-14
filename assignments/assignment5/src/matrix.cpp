/**
 * @file matrix.cpp
 * @brief Implementation of matrix operations for distributed GEMM.
 *
 * Provides initialization of matrix B and the core GEMM triple loop.
 * Matrix A is computed on-the-fly to save memory. Only boundary elements
 * of C are extracted for verification purposes.
 */

#include "assignment5/matrix.h"
#include <cstddef>

namespace a5 {

void init_B(std::vector<double>& B, int N) {
  const std::size_t total = static_cast<std::size_t>(N) * static_cast<std::size_t>(N);
  B.assign(total, 0.0);
  
  // Fill B with B[k][j] = 1.0 / (j + 1)
  for (int k = 0; k < N; ++k) {
    const std::size_t row_start = static_cast<std::size_t>(k) * static_cast<std::size_t>(N);
    for (int j = 0; j < N; ++j) {
      B[row_start + static_cast<std::size_t>(j)] = 1.0 / static_cast<double>(j + 1);
    }
  }
}

void compute_local_rows(
    int N, int row_offset, int row_count, const std::vector<double>& B,
    double* c00, double* c0N1, double* cN10, double* cN1N1) {
  
  // Iterate over each local row
  for (int li = 0; li < row_count; ++li) {
    const int i_glob = row_offset + li;  // Global row index
    
    // Compute each element in this row: C[i_glob][j] = sum_k A[i_glob][k] * B[k][j]
    for (int j = 0; j < N; ++j) {
      double sum = 0.0;
      
      // Triple loop: C = A * B, with A[i][k] = (i + 1) computed on-the-fly
      for (int k = 0; k < N; ++k) {
        const std::size_t idx_B = static_cast<std::size_t>(k) * static_cast<std::size_t>(N)
                                + static_cast<std::size_t>(j);
        const double a_ik = static_cast<double>(i_glob + 1);
        sum += a_ik * B[idx_B];
      }
      
      // Store boundary elements if this rank owns them
      if (i_glob == 0 && j == 0 && c00) {
        *c00 = sum;
      }
      if (i_glob == 0 && j == N - 1 && c0N1) {
        *c0N1 = sum;
      }
      if (i_glob == N - 1 && j == 0 && cN10) {
        *cN10 = sum;
      }
      if (i_glob == N - 1 && j == N - 1 && cN1N1) {
        *cN1N1 = sum;
      }
    }
  }
}

bool exceeds_memory_budget_for_B(int N, std::size_t threshold_bytes) {
  // Compute required memory for N x N doubles (8 bytes each)
  // Use size_t to avoid overflow for large N
  const std::size_t n = static_cast<std::size_t>(N);
  const std::size_t bytes = 8 * n * n;
  return bytes > threshold_bytes;
}

} // namespace a5
