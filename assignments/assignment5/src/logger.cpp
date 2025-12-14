#include "assignment5/logger.h"
#include <iostream>

namespace a5 {

void log_info_root(int rank, const std::string& msg) {
  if (rank == 0) { std::cout << "[INFO] " << msg << std::endl; }
}
void log_error_all(int, const std::string& msg) {
  std::cerr << "[ERROR] " << msg << std::endl;
}

} // namespace a5
