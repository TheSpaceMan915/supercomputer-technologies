/**
 * @file main.cpp
 * @brief Entry point for assignment5: distributed row-block MPI GEMM.
 *
 * Implements a parallel dense matrix multiplication C = A * B using MPI
 * with a simple row-block distribution. Matrix B is broadcast to all ranks,
 * and each rank computes its assigned rows of C. Only four boundary elements
 * are collected for verification.
 *
 * Usage: mpirun -np <P> assignment5 <N> [--iters k]
 */

#include <mpi.h>
#include <vector>
#include <string>
#include <sstream>
#include <cstddef>

#include "assignment5/cli.h"
#include "assignment5/logger.h"
#include "assignment5/dist.h"
#include "assignment5/matrix.h"

/**
 * @brief Send a scalar value to rank 0 if this rank owns it.
 *
 * Helper function to conditionally send boundary elements from non-root ranks.
 * Only the owning rank performs the send operation.
 *
 * @param rank  Current rank
 * @param owner Rank that owns this value
 * @param value The scalar to send
 * @param tag   MPI message tag
 */
static void send_scalar_if_owner(int rank, int owner, double value, int tag) {
  if (rank == owner) {
    MPI_Send(&value, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
  }
}

/**
 * @brief Receive boundary elements from owning ranks (rank 0 only).
 *
 * Collects the four corner elements of C from their respective owners.
 * If rank 0 owns a value, it doesn't perform a receive for that element.
 *
 * @param own0  Rank that owns row 0
 * @param ownN  Rank that owns row N-1
 * @param c00   Reference to C[0][0]
 * @param c0N1  Reference to C[0][N-1]
 * @param cN10  Reference to C[N-1][0]
 * @param cN1N1 Reference to C[N-1][N-1]
 */
static void receive_boundary_elements(
    int own0, int ownN,
    double& c00, double& c0N1, double& cN10, double& cN1N1) {
  
  if (own0 != 0) {
    MPI_Recv(&c00, 1, MPI_DOUBLE, own0, 101, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&c0N1, 1, MPI_DOUBLE, own0, 102, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  if (ownN != 0) {
    MPI_Recv(&cN10, 1, MPI_DOUBLE, ownN, 103, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&cN1N1, 1, MPI_DOUBLE, ownN, 104, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
}

/**
 * @brief Log the four boundary elements of matrix C (rank 0 only).
 *
 * Formats and prints the corner values of C for verification.
 *
 * @param rank  Current rank (only rank 0 will log)
 * @param N     Matrix dimension
 * @param c00   Value of C[0][0]
 * @param c0N1  Value of C[0][N-1]
 * @param cN10  Value of C[N-1][0]
 * @param cN1N1 Value of C[N-1][N-1]
 */
static void log_boundary_values(
    int rank, int N,
    double c00, double c0N1, double cN10, double cN1N1) {
  
  if (rank == 0) {
    std::ostringstream oss;
    oss.setf(std::ios::fixed);
    oss.precision(8);
    oss << "C[0][0]=" << c00
        << " C[0][" << (N - 1) << "]=" << c0N1
        << " C[" << (N - 1) << "][0]=" << cN10
        << " C[" << (N - 1) << "][" << (N - 1) << "]=" << cN1N1;
    a5::log_info_root(rank, oss.str());
  }
}

/**
 * @brief Log performance metrics (rank 0 only).
 *
 * Computes and logs elapsed time, total floating-point operations,
 * and GFLOPS achieved.
 *
 * @param rank      Current rank
 * @param N         Matrix dimension
 * @param elapsed_s Average elapsed time per iteration (seconds)
 */
static void log_performance(int rank, int N, double elapsed_s) {
  if (rank == 0) {
    const double elapsed_ms = elapsed_s * 1000.0;
    const double flops = 2.0 * static_cast<double>(N) 
                            * static_cast<double>(N) 
                            * static_cast<double>(N);
    const double gflops = (elapsed_s > 0.0) ? (flops / (elapsed_s * 1e9)) : 0.0;
    
    std::ostringstream oss;
    oss.setf(std::ios::fixed);
    oss.precision(3);
    oss << "elapsed_ms=" << elapsed_ms
        << " flops=" << flops
        << " gflops=" << gflops;
    a5::log_info_root(rank, oss.str());
  }
}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  
  int rank = 0;
  int size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
  // Parse command-line arguments
  a5::Options opt;
  std::string err;
  if (!a5::parse_cli(argc, argv, opt, err)) {
    if (rank == 0) {
      a5::log_error_all(rank, err);
    }
    MPI_Finalize();
    return 1;
  }
  
  const int N = opt.N;
  const int iters = opt.iters;
  
  a5::log_info_root(rank, "assignment5 start");
  {
    std::ostringstream oss;
    oss << "N=" << N << " iters=" << iters << " ranks=" << size << " dist=row-block";
    a5::log_info_root(rank, oss.str());
  }
  
  // Guard against excessive memory allocation for B (1 GiB limit)
  const std::size_t memory_limit = static_cast<std::size_t>(1) << 30;
  if (a5::exceeds_memory_budget_for_B(N, memory_limit)) {
    if (rank == 0) {
      a5::log_error_all(rank, "N too large for B (memory guard)");
    }
    MPI_Finalize();
    return 2;
  }
  
  // Allocate and initialize matrix B (rank 0), then broadcast to all
  std::vector<double> B;
  const std::size_t B_size = static_cast<std::size_t>(N) * static_cast<std::size_t>(N);
  B.resize(B_size);
  if (rank == 0) {
    a5::init_B(B, N);
  }
  MPI_Bcast(&B[0], static_cast<int>(B.size()), MPI_DOUBLE, 0, MPI_COMM_WORLD);
  
  // Compute row partition for this rank
  int row_offset = 0;
  int row_count = 0;
  a5::row_block_partition(N, size, rank, row_offset, row_count);
  
  // Determine which ranks own the boundary rows
  const int owner_row0 = a5::owner_of_row(N, size, 0);
  const int owner_rowN = a5::owner_of_row(N, size, N - 1);
  
  // Storage for boundary elements
  double c00 = 0.0, c0N1 = 0.0, cN10 = 0.0, cN1N1 = 0.0;
  
  // Timing loop: compute C = A * B for 'iters' iterations
  MPI_Barrier(MPI_COMM_WORLD);
  double t_start = MPI_Wtime();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Set up pointers for boundary elements (NULL if not owned)
    double* p_c00   = (rank == owner_row0) ? &c00   : static_cast<double*>(0);
    double* p_c0N1  = (rank == owner_row0) ? &c0N1  : static_cast<double*>(0);
    double* p_cN10  = (rank == owner_rowN) ? &cN10  : static_cast<double*>(0);
    double* p_cN1N1 = (rank == owner_rowN) ? &cN1N1 : static_cast<double*>(0);
    
    a5::compute_local_rows(N, row_offset, row_count, B,
                           p_c00, p_c0N1, p_cN10, p_cN1N1);
    
    MPI_Barrier(MPI_COMM_WORLD);
  }
  
  double t_end = MPI_Wtime();
  const double elapsed_s = (t_end - t_start) / (iters > 0 ? iters : 1);
  
  // Collect boundary elements at rank 0
  if (rank != 0) {
    // Non-root ranks send their owned boundary elements
    if (rank == owner_row0) {
      send_scalar_if_owner(rank, owner_row0, c00, 101);
      send_scalar_if_owner(rank, owner_row0, c0N1, 102);
    }
    if (rank == owner_rowN) {
      send_scalar_if_owner(rank, owner_rowN, cN10, 103);
      send_scalar_if_owner(rank, owner_rowN, cN1N1, 104);
    }
  } else {
    // Rank 0 receives from other ranks if they own the elements
    receive_boundary_elements(owner_row0, owner_rowN, c00, c0N1, cN10, cN1N1);
  }
  
  // Log results (rank 0 only)
  log_boundary_values(rank, N, c00, c0N1, cN10, cN1N1);
  log_performance(rank, N, elapsed_s);
  a5::log_info_root(rank, "assignment5 done");
  
  MPI_Finalize();
  return 0;
}
