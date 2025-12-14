/**
 * @file cli.h
 * @brief Command-line argument parsing for assignment5 MPI GEMM.
 *
 * Provides a simple parser for matrix size N and iteration count,
 * supporting both positional arguments and named options (--iters).
 */

#ifndef ASSIGNMENT5_CLI_H
#define ASSIGNMENT5_CLI_H

#include <string>

namespace a5 {

/**
 * @brief Configuration options parsed from command-line arguments.
 *
 * Holds the matrix dimension N and the number of benchmark iterations.
 * Default values ensure a valid state if parsing is incomplete.
 */
struct Options {
  int N;       ///< Matrix dimension (N x N matrices A, B, and C)
  int iters;   ///< Number of iterations for timing benchmarks
  
  Options() : N(0), iters(1) {}
};

/**
 * @brief Parse command-line arguments into Options struct.
 *
 * Expects at least one positional argument: the matrix size N.
 * Optionally accepts --iters <k> to set the iteration count.
 *
 * @param argc Argument count from main()
 * @param argv Argument vector from main()
 * @param out  Output structure to populate with parsed values
 * @param err  Error message if parsing fails
 * @return true if parsing succeeded, false otherwise
 */
bool parse_cli(int argc, char** argv, Options& out, std::string& err);

} // namespace a5

#endif
