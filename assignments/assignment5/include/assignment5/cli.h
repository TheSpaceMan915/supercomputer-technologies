#ifndef ASSIGNMENT5_CLI_H
#define ASSIGNMENT5_CLI_H

#include <string>

namespace a5 {

struct Options {
  int N;
  int iters;
  Options() : N(0), iters(1) {}
};

bool parse_cli(int argc, char** argv, Options& out, std::string& err);

} // namespace a5

#endif
