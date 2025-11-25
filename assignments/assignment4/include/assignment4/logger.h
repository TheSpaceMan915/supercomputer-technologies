// Simple rank-aware logging: root-only info, all-ranks error.
// Ensures only rank 0 prints results; errors go to stderr for all ranks.
// Depends on: <string>, <iostream>, <cstdio>.

#ifndef ASSIGNMENT4_LOGGER_H
#define ASSIGNMENT4_LOGGER_H

#include <string>

namespace assignment4 {

// Logs info message to stdout only if rank == 0.
// Prevents duplicate output in multi-rank MPI runs.
void log_info_root(int rank, const std::string& msg);

// Logs error message to stderr unconditionally.
// All ranks can report errors for debugging.
void log_error(const std::string& msg);

} // namespace assignment4

#endif
