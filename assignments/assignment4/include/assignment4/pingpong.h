// MPI ping-pong latency measurement between rank 0 and rank 1.
// Uses MPI_Send/MPI_Recv for synchronous point-to-point timing.
// Depends on: <vector>, MPI, Options from cli.h.

#ifndef ASSIGNMENT4_PINGPONG_H
#define ASSIGNMENT4_PINGPONG_H

#include <vector>

namespace assignment4 {

struct Options;

// Runs ping-pong latency benchmark for each size in 'sizes'.
// Preconditions: world == 2, rank in {0,1}, MPI_Init already called.
// Rank 0 sends/receives and measures round-trip time; rank 1 echoes silently.
// Returns 0 on success, 1 on error (calls MPI_Abort on fatal errors).
// Latency = (round_trip_time / (2 * iters)) * 1e6 microseconds.
int run_pingpong(const std::vector<int>& sizes,
                 const Options& opt,
                 int rank,
                 int world);

} // namespace assignment4

#endif
