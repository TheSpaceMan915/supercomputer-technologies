// logger.h - Simple logging utilities for INFO and ERROR messages
// Provides thread-unsafe logging to stdout/stderr; suitable for single-threaded CLI apps.
// Dependencies: std::string, iostream

#ifndef ASSIGNMENT1_LOGGER_H
#define ASSIGNMENT1_LOGGER_H

#include <string>

namespace assignment1 {

// Log an informational message to stdout with [INFO] prefix
void log_info(const std::string& msg);

// Log an error message to stderr with [ERROR] prefix
void log_error(const std::string& msg);

} // namespace assignment1

#endif // ASSIGNMENT1_LOGGER_H
