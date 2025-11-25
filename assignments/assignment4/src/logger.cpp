// Minimal logging utilities for MPI applications.
// log_info_root: only rank 0 prints to stdout (avoids duplicate output).
// log_error: all ranks print to stderr for debugging.

#include "assignment4/logger.h"
#include <iostream>
#include <cstdio>

namespace assignment4 {

void log_info_root(int rank, const std::string& msg)
{
    if (rank == 0) {
        std::cout << "[INFO] " << msg << std::endl;
    }
}

void log_error(const std::string& msg)
{
    std::fprintf(stderr, "[ERROR] %s\n", msg.c_str());
}

} // namespace assignment4
