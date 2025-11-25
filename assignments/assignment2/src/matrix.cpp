/*
 * matrix.cpp — Square matrix operations in row-major layout
 * Provides basic container, initialization routines, and naive O(N^3) multiply.
 * Uses flat vector storage for C++98 compatibility; no cache-friendly blocking.
 */
#include "assignment2/matrix.h"
#include <stdexcept>

namespace assignment2 {

// Construct n×n matrix initialized to zero; throws if n <= 0
Matrix::Matrix(int size) : n(size), data()
{
  if (n <= 0) throw std::invalid_argument("Matrix size must be > 0");
  // Cast to size_type to avoid signed overflow for large N
  data.assign(static_cast<std::vector<double>::size_type>(n) *
              static_cast<std::vector<double>::size_type>(n), 0.0);
}

// Row-major indexing: row i, column j → data[i*n + j]
double& Matrix::at(int i, int j)
{
  return data[static_cast<std::vector<double>::size_type>(i * n + j)];
}

const double& Matrix::at(int i, int j) const
{
  return data[static_cast<std::vector<double>::size_type>(i * n + j)];
}

// Initialize A[i][j] = i+1 (same value along each row for testing)
void initA(Matrix& A)
{
  const int N = A.n;
  for (int i = 0; i < N; ++i) {
    const double v = static_cast<double>(i + 1);
    for (int j = 0; j < N; ++j) {
      A.at(i, j) = v;
    }
  }
}

// Initialize B[i][j] = 1/(j+1) (same value down each column for testing)
void initB(Matrix& B)
{
  const int N = B.n;
  for (int j = 0; j < N; ++j) {
    const double v = 1.0 / static_cast<double>(j + 1);
    for (int i = 0; i < N; ++i) {
      B.at(i, j) = v;
    }
  }
}

// Classic triple-loop C = A·B: 2*N^3 FLOPs; no cache optimization
// Throws if matrices have mismatched dimensions
void multiply(const Matrix& A, const Matrix& B, Matrix& C)
{
  const int N = A.n;
  if (B.n != N || C.n != N) throw std::invalid_argument("Dimension mismatch");
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      double acc = 0.0;
      for (int k = 0; k < N; ++k) {
        acc += A.at(i, k) * B.at(k, j);
      }
      C.at(i, j) = acc;
    }
  }
}

} // namespace assignment2
