/*
 * matrix.h — Square matrix container and triple-loop multiplication
 * Provides row-major double storage via std::vector and C = A · B operation.
 * Invariants: Matrix.n > 0 and data.size() == n*n.
 */
#ifndef ASSIGNMENT2_MATRIX_H
#define ASSIGNMENT2_MATRIX_H

#include <vector>

namespace assignment2 {

// Row-major n×n matrix using a flat vector for C++98 compatibility
struct Matrix {
  int n;
  std::vector<double> data;
  explicit Matrix(int size);
  double& at(int i, int j);
  const double& at(int i, int j) const;
};

// Initialize A with A[i][j] = i+1 for closed-form testing
void initA(Matrix& A);

// Initialize B with B[i][j] = 1/(j+1) for closed-form testing
void initB(Matrix& B);

// Classic O(N^3) triple-loop matrix multiply: C = A·B
void multiply(const Matrix& A, const Matrix& B, Matrix& C);

} // namespace assignment2

#endif // ASSIGNMENT2_MATRIX_H
