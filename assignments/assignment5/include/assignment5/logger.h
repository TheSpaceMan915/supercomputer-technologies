/**
 * @file logger.h
 * @brief Simple rank-aware logging utilities for MPI programs.
 *
 * Provides INFO and ERROR logging functions that conditionally output
 * messages based on the caller's rank, simplifying MPI output coordination.
 */

#ifndef ASSIGNMENT5_LOGGER_H
#define ASSIGNMENT5_LOGGER_H

#include <string>

namespace a5 {

/**
 * @brief Log an informational message (rank 0 only).
 *
 * Only rank 0 will print the message to stdout. Other ranks silently ignore.
 * Use this to avoid duplicated output when all ranks execute the same code.
 *
 * @param rank Current MPI rank
 * @param msg  Message to log (will be prefixed with "[INFO] ")
 */
void log_info_root(int rank, const std::string& msg);

/**
 * @brief Log an error message (all ranks).
 *
 * All ranks print the message to stderr. Useful for error conditions where
 * each rank may have different error details or when debugging distributed issues.
 *
 * @param rank Current MPI rank (currently unused, but kept for symmetry)
 * @param msg  Error message to log (will be prefixed with "[ERROR] ")
 */
void log_error_all(int rank, const std::string& msg);

} // namespace a5

#endif
