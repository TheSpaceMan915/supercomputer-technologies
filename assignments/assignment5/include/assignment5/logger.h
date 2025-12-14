#ifndef ASSIGNMENT5_LOGGER_H
#define ASSIGNMENT5_LOGGER_H

#include <string>

namespace a5 {

void log_info_root(int rank, const std::string& msg);
void log_error_all(int rank, const std::string& msg);

} // namespace a5

#endif
