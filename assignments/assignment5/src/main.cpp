// See README for usage and build notes.
#include <mpi.h>
#include <vector>
#include <string>
#include <sstream>

#include "assignment5/cli.h"
#include "assignment5/logger.h"
#include "assignment5/dist.h"
#include "assignment5/matrix.h"

static void send_scalar_if_owner(int rank, int owner, double value, int tag) {
  if (rank == owner) { MPI_Send(&value, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD); }
}

int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int rank=0, size=1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  a5::Options opt; std::string err;
  if (!a5::parse_cli(argc, argv, opt, err)) { if (rank==0) a5::log_error_all(rank, err); MPI_Finalize(); return 1; }
  const int N = opt.N; const int iters = opt.iters;

  a5::log_info_root(rank, "assignment5 start");
  { std::ostringstream s; s << "N="<<N<<" iters="<<iters<<" ranks="<<size<<" dist=row-block"; a5::log_info_root(rank, s.str()); }

  const unsigned long long limit = 1ULL<<30;
  if (a5::exceeds_memory_budget_for_B(N, limit)) { if (rank==0) a5::log_error_all(rank,"N too large for B (memory guard)"); MPI_Finalize(); return 2; }

  std::vector<double> B; B.resize((size_t)N*(size_t)N);
  if (rank==0) a5::init_B(B, N);
  MPI_Bcast(&B[0], (int)B.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

  int off=0, cnt=0; a5::row_block_partition(N, size, rank, off, cnt);
  const int own0 = a5::owner_of_row(N, size, 0);
  const int ownN = a5::owner_of_row(N, size, N-1);
  double c00=0, c0N1=0, cN10=0, cN1N1=0;

  MPI_Barrier(MPI_COMM_WORLD);
  double t0 = MPI_Wtime();
  for (int it=0; it<iters; ++it) {
    double* p00   = (rank==own0 ? &c00   : (double*)0);
    double* p0N1  = (rank==own0 ? &c0N1  : (double*)0);
    double* pN10  = (rank==ownN ? &cN10  : (double*)0);
    double* pN1N1 = (rank==ownN ? &cN1N1 : (double*)0);
    a5::compute_local_rows(N, off, cnt, B, p00, p0N1, pN10, pN1N1);
    MPI_Barrier(MPI_COMM_WORLD);
  }
  double t1 = MPI_Wtime();
  double elapsed_s = (t1 - t0) / (iters>0?iters:1);
  double elapsed_ms = elapsed_s * 1000.0;

  if (rank!=0) {
    if (rank==own0) { send_scalar_if_owner(rank, own0, c00, 101); send_scalar_if_owner(rank, own0, c0N1, 102); }
    if (rank==ownN) { send_scalar_if_owner(rank, ownN, cN10,103); send_scalar_if_owner(rank, ownN, cN1N1,104); }
  } else {
    if (own0!=0)  MPI_Recv(&c00,  1, MPI_DOUBLE, own0, 101, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (own0!=0)  MPI_Recv(&c0N1, 1, MPI_DOUBLE, own0, 102, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (ownN!=0)  MPI_Recv(&cN10, 1, MPI_DOUBLE, ownN, 103, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (ownN!=0)  MPI_Recv(&cN1N1,1, MPI_DOUBLE, ownN, 104, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  if (rank==0) {
    std::ostringstream b; b.setf(std::ios::fixed); b.precision(8);
    b << "C[0][0]="<<c00
      << " C[0]["<<(N-1)<<"]="<<c0N1
      << " C["<<(N-1)<<"][0]="<<cN10
      << " C["<<(N-1)<<"]["<<(N-1)<<"]="<<cN1N1;
    a5::log_info_root(rank, b.str());
  }

  const double flops = 2.0 * (double)N * (double)N * (double)N;
  double gflops = (elapsed_s>0.0) ? flops/(elapsed_s*1e9) : 0.0;
  if (rank==0) {
    std::ostringstream p; p.setf(std::ios::fixed); p.precision(3);
    p << "elapsed_ms="<<elapsed_ms<<" flops="<<flops<<" gflops="<<gflops;
    a5::log_info_root(rank, p.str());
    a5::log_info_root(rank, "assignment5 done");
  }

  MPI_Finalize();
  return 0;
}
