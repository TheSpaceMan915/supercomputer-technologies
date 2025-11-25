// MPI ping-pong latency benchmark: rank 0 sends to rank 1, rank 1 echoes back.
// Measures one-way latency from round-trip time over multiple iterations.
// Uses MPI_Send/MPI_Recv for synchronous point-to-point communication.

#include "assignment4/pingpong.h"
#include "assignment4/logger.h"
#include "assignment4/cli.h"

#include <vector>
#include <string>
#include <sstream>
#include <new>

#include <mpi.h>

namespace assignment4 {

// Returns current wall-clock time in seconds via MPI_Wtime().
static double now_sec() { return MPI_Wtime(); }

int run_pingpong(const std::vector<int>& sizes,
                 const Options& opt,
                 int rank,
                 int world)
{
    // Ping-pong requires exactly 2 ranks
    if (world != 2) {
        if (rank == 0) {
            std::ostringstream oss;
            oss << "world size must be 2 (got " << world << ")";
            log_error(oss.str());
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    // Distinct tags for ping and pong directions
    const int TAG_PING = 100;
    const int TAG_PONG = 101;

    {
        std::ostringstream oss;
        oss << "ranks=" << world
            << " warmup=" << opt.warmup
            << " iters=" << opt.iters
            << " min=" << (sizes.empty() ? 0 : sizes.front())
            << " max=" << (sizes.empty() ? 0 : sizes.back())
            << " factor=" << opt.factor;
        log_info_root(rank, oss.str());
        log_info_root(rank, "mode=ping-pong");
    }

    // Loop over each message size
    for (std::size_t i = 0; i < sizes.size(); ++i) {
        const int bytes = sizes[i];

        // Allocate buffer (both ranks need same size)
        std::vector<char> buf;
        try { buf.resize(bytes); }
        catch (const std::bad_alloc&) {
            if (rank == 0) {
                std::ostringstream oss; oss << "allocation failed for size=" << bytes;
                log_error(oss.str());
            }
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }

        MPI_Barrier(MPI_COMM_WORLD);

        // Warm-up iterations: prime cache and network
        for (int w = 0; w < opt.warmup; ++w) {
            if (rank == 0) {
                MPI_Send(&buf[0], bytes, MPI_BYTE, 1, TAG_PING, MPI_COMM_WORLD);
                MPI_Recv(&buf[0], bytes, MPI_BYTE, 1, TAG_PONG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else {
                MPI_Recv(&buf[0], bytes, MPI_BYTE, 0, TAG_PING, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&buf[0], bytes, MPI_BYTE, 0, TAG_PONG, MPI_COMM_WORLD);
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);

        // Measured iterations: rank 0 times the round-trip
        double t0 = 0.0, t1 = 0.0;
        if (rank == 0) t0 = now_sec();

        for (int it = 0; it < opt.iters; ++it) {
            if (rank == 0) {
                MPI_Send(&buf[0], bytes, MPI_BYTE, 1, TAG_PING, MPI_COMM_WORLD);
                MPI_Recv(&buf[0], bytes, MPI_BYTE, 1, TAG_PONG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else {
                MPI_Recv(&buf[0], bytes, MPI_BYTE, 0, TAG_PING, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&buf[0], bytes, MPI_BYTE, 0, TAG_PONG, MPI_COMM_WORLD);
            }
        }

        if (rank == 0) {
            t1 = now_sec();
            const double rt = (t1 > t0) ? (t1 - t0) : 0.0;
            // One-way latency = round_trip / (2 * iterations), converted to microseconds
            const double one_way_us = (opt.iters > 0) ? (rt / (2.0 * opt.iters) * 1e6) : 0.0;

            std::ostringstream oss;
            oss.setf(std::ios::fixed);
            oss.precision(2);
            oss << "size=" << bytes << " B latency_us=" << one_way_us;
            log_info_root(rank, oss.str());
        }
    }

    return 0;
}

} // namespace assignment4
