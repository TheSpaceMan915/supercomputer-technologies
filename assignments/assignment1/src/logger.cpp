// logger.cpp - Simple logging implementation
// Thread-unsafe; writes INFO to stdout, ERROR to stderr with prefixes.

#include "assignment1/logger.h"
#include <iostream>

namespace assignment1 {

void log_info(const std::string& msg) { std::cout << "[INFO] " << msg << std::endl; }
void log_error(const std::string& msg) { std::cerr << "[ERROR] " << msg << std::endl; }

} // namespace assignment1
