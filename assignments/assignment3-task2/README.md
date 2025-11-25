# assignment3-task2 — Parallel Matrix Multiply (C++98, OpenMP)

This assignment computes `C = A · B` for two N×N matrices:

- `A[i][j] = i + 1`
- `B[i][j] = 1.0 / (j + 1)`
- Storage: `std::vector<double>` with index `i*N + j`
- Algorithm: classic triple-loop `O(N^3)` multiplication

When built with OpenMP (3.0 or later), the outer loop is parallelized.
When OpenMP is not available, the code falls back to a serial implementation.
