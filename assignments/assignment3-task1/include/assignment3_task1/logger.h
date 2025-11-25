// logger.h — Simple logging utilities for assignment3-task1
// Provides info/error logging to stdout/stderr with prefixed labels.
// Thread-safe if stdout/stderr are thread-safe (standard streams usually are).

#ifndef ASSIGNMENT3_TASK1_LOGGER_H
#define ASSIGNMENT3_TASK1_LOGGER_H

#include <string>

namespace assignment3_task1 {

// Log an informational message to stdout with [INFO] prefix
void log_info(const std::string& msg);

// Log an error message to stderr with [ERROR] prefix
void log_error(const std::string& msg);

}  // namespace assignment3_task1

#endif  // ASSIGNMENT3_TASK1_LOGGER_H
