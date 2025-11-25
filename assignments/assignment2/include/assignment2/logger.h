/*
 * logger.h — Simple console logging utilities
 * Routes info to stdout with [INFO] prefix and errors to stderr with [ERROR] prefix.
 */
#ifndef ASSIGNMENT2_LOGGER_H
#define ASSIGNMENT2_LOGGER_H

#include <string>

namespace assignment2 {
// Write [INFO] message to stdout
void log_info(const std::string& msg);

// Write [ERROR] message to stderr
void log_error(const std::string& msg);
}

#endif // ASSIGNMENT2_LOGGER_H
