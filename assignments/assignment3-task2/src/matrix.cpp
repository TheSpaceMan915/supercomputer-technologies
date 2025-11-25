/* matrix.cpp: Dense matrix operations with OpenMP parallelization.
 * Implements initialization and multiplication for row-major N×N matrices.
 * Parallel multiplication distributes rows across threads with static scheduling.
 */
#include "assignment3_task2/matrix.h"

#ifdef _OPENMP
#include <omp.h>
#endif

namespace assignment3_task2
{
    // Convert 2D index (i,j) to 1D row-major offset.
    static inline int idx(int N, int i, int j)
    {
        return i * N + j;
    }

    void init_A(std::vector<double>& A, int N)
    {
        A.resize(N * N);
        for (int i = 0; i < N; ++i)
        {
            const double base = static_cast<double>(i + 1);
            for (int j = 0; j < N; ++j)
            {
                A[idx(N, i, j)] = base;  // Row i has constant value i+1
            }
        }
    }

    void init_B(std::vector<double>& B, int N)
    {
        B.resize(N * N);
        for (int j = 0; j < N; ++j)
        {
            const double col = 1.0 / static_cast<double>(j + 1);
            for (int i = 0; i < N; ++i)
            {
                B[idx(N, i, j)] = col;  // Column j has constant value 1/(j+1)
            }
        }
    }

    void multiply_serial(const std::vector<double>& A,
                         const std::vector<double>& B,
                         std::vector<double>& C,
                         int N)
    {
        C.assign(N * N, 0.0);

        // Classic triple-loop: C[i][j] = sum_k A[i][k] * B[k][j]
        for (int i = 0; i < N; ++i)
        {
            const int row = i * N;  // Pre-compute row offset for A and C
            for (int j = 0; j < N; ++j)
            {
                double sum = 0.0;
                for (int k = 0; k < N; ++k)
                {
                    sum += A[row + k] * B[k * N + j];
                }
                C[row + j] = sum;
            }
        }
    }

    void multiply_parallel(const std::vector<double>& A,
                           const std::vector<double>& B,
                           std::vector<double>& C,
                           int N)
    {
        C.assign(N * N, 0.0);

#if defined(_OPENMP)
        // Parallelize outer loop over rows with static scheduling.
        // Each thread computes independent rows (no write conflicts).
        // A, B are shared (read-only); sum is private; C writes are non-overlapping.
        // Number of threads controlled by OMP_NUM_THREADS environment variable.
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < N; ++i)
        {
            const int row = i * N;
            for (int j = 0; j < N; ++j)
            {
                double sum = 0.0;  // Each thread has private sum
                for (int k = 0; k < N; ++k)
                {
                    sum += A[row + k] * B[k * N + j];
                }
                C[row + j] = sum;  // Each thread writes to distinct C elements
            }
        }
#else
        // Fallback to serial when OpenMP not available at compile time
        multiply_serial(A, B, C, N);
#endif
    }
}
