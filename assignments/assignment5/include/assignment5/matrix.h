/**
 * @file matrix.h
 * @brief Matrix initialization and computation kernels for distributed GEMM.
 *
 * Provides initialization of matrix B and the core GEMM computation for
 * local row blocks. Also includes a memory budget check to guard against
 * excessive allocations.
 */

#ifndef ASSIGNMENT5_MATRIX_H
#define ASSIGNMENT5_MATRIX_H

#include <vector>
#include <cstddef>

namespace a5 {

/**
 * @brief Initialize matrix B with the formula B[k][j] = 1.0 / (j + 1).
 *
 * Allocates and fills an N x N matrix stored in row-major order in a
 * single std::vector. This initialization yields a predictable result
 * when multiplied with A[i][k] = (i + 1): C[i][j] = N * (i + 1) / (j + 1).
 *
 * @param B Output vector to populate (will be resized to N*N elements)
 * @param N Dimension of the square matrix
 */
void init_B(std::vector<double>& B, int N);

/**
 * @brief Compute local rows of C = A * B using the standard triple loop.
 *
 * Each rank computes a subset of rows. The values of A are computed on the fly
 * (A[i][k] = i + 1) to save memory. Only the four boundary elements of C
 * (corners) are stored and returned to the caller.
 *
 * @param N          Matrix dimension (N x N)
 * @param row_offset Global starting row index for this rank
 * @param row_count  Number of rows to compute locally
 * @param B          The full N x N matrix B (broadcast to all ranks)
 * @param c00        Pointer to store C[0][0], or NULL if not owned
 * @param c0N1       Pointer to store C[0][N-1], or NULL if not owned
 * @param cN10       Pointer to store C[N-1][0], or NULL if not owned
 * @param cN1N1      Pointer to store C[N-1][N-1], or NULL if not owned
 */
void compute_local_rows(
    int N,
    int row_offset,
    int row_count,
    const std::vector<double>& B,
    double* c00,
    double* c0N1,
    double* cN10,
    double* cN1N1);

/**
 * @brief Check if matrix B exceeds a memory threshold.
 *
 * Computes the memory required to store an N x N matrix of doubles
 * (8 bytes per element) and compares it to the given threshold.
 * Helps prevent out-of-memory errors or excessive allocations.
 *
 * @param N                 Matrix dimension
 * @param threshold_bytes   Maximum allowed memory in bytes
 * @return true if memory requirement exceeds threshold, false otherwise
 */
bool exceeds_memory_budget_for_B(int N, std::size_t threshold_bytes);

} // namespace a5

#endif
