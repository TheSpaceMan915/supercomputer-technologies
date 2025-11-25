/* logger.h: Simple message logging for assignment3-task2.
 * Thread-safe via cout/cerr atomicity (each call outputs single line).
 */
#ifndef ASSIGNMENT3_TASK2_LOGGER_H
#define ASSIGNMENT3_TASK2_LOGGER_H

#include <string>

namespace assignment3_task2
{
    // Log informational message to stdout with [INFO] prefix.
    void log_info(const std::string& msg);

    // Log error message to stderr with [ERROR] prefix.
    void log_error(const std::string& msg);
}

#endif
