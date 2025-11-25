// logger.cpp — Implementation of logging utilities
// Simple wrappers around cout/cerr with prefixed labels.

#include "assignment3_task1/logger.h"
#include <iostream>

namespace assignment3_task1 {

void log_info(const std::string& msg) {
  std::cout << "[INFO] " << msg << std::endl;
}

void log_error(const std::string& msg) {
  std::cerr << "[ERROR] " << msg << std::endl;
}

}  // namespace assignment3_task1
