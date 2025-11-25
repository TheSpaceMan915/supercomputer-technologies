/* matrix.h: N×N dense matrix operations with row-major layout.
 * Provides matrix initialization and multiplication (serial & parallel).
 * Matrices stored as std::vector<double>, indexed i*N + j.
 */
#ifndef ASSIGNMENT3_TASK2_MATRIX_H
#define ASSIGNMENT3_TASK2_MATRIX_H

#include <vector>

namespace assignment3_task2
{
    // Initialize matrix A where A[i][j] = i + 1 (rows have constant values).
    void init_A(std::vector<double>& A, int N);

    // Initialize matrix B where B[i][j] = 1/(j+1) (columns have constant values).
    void init_B(std::vector<double>& B, int N);

    // Compute C = A * B using classic O(N^3) triple-loop (single-threaded).
    // C is zeroed first; A and B are read-only.
    void multiply_serial(const std::vector<double>& A,
                         const std::vector<double>& B,
                         std::vector<double>& C,
                         int N);

    // Compute C = A * B with outer loop parallelized via OpenMP (if available).
    // Falls back to serial multiplication when OpenMP is not available.
    // Each thread computes independent rows, avoiding write conflicts.
    void multiply_parallel(const std::vector<double>& A,
                           const std::vector<double>& B,
                           std::vector<double>& C,
                           int N);
}

#endif
