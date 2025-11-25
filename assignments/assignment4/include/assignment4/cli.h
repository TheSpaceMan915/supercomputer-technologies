// Command-line interface: parses ping-pong benchmark options.
// Validates ranges and stores defaults (warmup=10, iters=100, min=4B, max=10MiB, factor=2).
// Depends on: <string> for error reporting.

#ifndef ASSIGNMENT4_CLI_H
#define ASSIGNMENT4_CLI_H

#include <string>

namespace assignment4 {

// Configuration for MPI ping-pong latency runs.
// Defaults provide reasonable warm-up and iteration counts for stable measurements.
struct Options {
    int warmup;     // warm-up iterations (>=0); discarded for cache/network stability
    int iters;      // measured iterations (>0); averaged for latency
    int min_bytes;  // smallest message size (>=1)
    int max_bytes;  // largest message size (>=min_bytes)
    int factor;     // geometric growth factor (>=2); next_size = current * factor
    Options() : warmup(10), iters(100), min_bytes(4), max_bytes(10485760), factor(2) {}
};

// Parses command-line arguments into Options.
// Returns false on invalid arguments or constraint violations (e.g., min > max).
// On error, 'err' contains a human-readable message.
bool parse_cli(int argc, char** argv, Options& opt, std::string& err);

} // namespace assignment4

#endif
