// MPI ping-pong latency benchmark main entry point.
// Initializes MPI, parses CLI, generates sizes, runs benchmark, finalizes.
// Exits with 0 on success, 1 on errors. Only rank 0 prints results.

#include "assignment4/cli.h"
#include "assignment4/logger.h"
#include "assignment4/sizes.h"
#include "assignment4/pingpong.h"

#include <mpi.h>
#include <vector>
#include <string>
#include <sstream>
#include <cstdio>

using namespace assignment4;

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank = 0, world = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world);

    Options opt;
    std::string err;
    if (!parse_cli(argc, argv, opt, err)) {
        if (rank == 0) {
            log_error(err);
            std::fprintf(stderr, "Usage: assignment4 [--warmup 10] [--iters 100] [--min-bytes 4] [--max-bytes 10485760] [--factor 2]\n");
        }
        MPI_Finalize();
        return 1;
    }

    log_info_root(rank, "assignment4 start");

    std::vector<int> sizes;
    if (!make_sizes(opt.min_bytes, opt.max_bytes, opt.factor, sizes)) {
        if (rank == 0) log_error("invalid size parameters");
        MPI_Finalize();
        return 1;
    }

    const int rc = run_pingpong(sizes, opt, rank, world);

    if (rc == 0) {
        log_info_root(rank, "assignment4 done");
    }

    MPI_Finalize();
    return (rc == 0) ? 0 : 1;
}
