#include "assignment5/cli.h"
#include <cstdlib>
#include <cstring>

namespace a5 {

static bool parse_int(const char* s, int& out) {
  if (!s || !*s) return false;
  char* endp = 0;
  long v = std::strtol(s, &endp, 10);
  if (endp == s || *endp != '\0') return false;
  if (v < -2147483647L || v > 2147483647L) return false;
  out = static_cast<int>(v);
  return true;
}

bool parse_cli(int argc, char** argv, Options& out, std::string& err) {
  if (argc < 2) { err = "Usage: assignment5 <N> [--iters k]"; return false; }
  int i = 1;
  int N = 0;
  int iters = 1;
  bool haveN = false;
  while (i < argc) {
    const char* a = argv[i];
    if (a[0] == '-' && a[1] == '-') {
      if (std::strcmp(a, "--iters") == 0) {
        if (i + 1 >= argc) { err = "missing value for --iters"; return false; }
        if (!parse_int(argv[i+1], iters) || iters <= 0) { err = "invalid --iters"; return false; }
        i += 2;
      } else {
        err = std::string("unknown option: ") + a;
        return false;
      }
    } else {
      if (haveN) { err = "unexpected positional argument"; return false; }
      if (!parse_int(a, N) || N <= 0) { err = "invalid N"; return false; }
      haveN = true;
      ++i;
    }
  }
  if (!haveN) { err = "missing N"; return false; }
  out.N = N; out.iters = iters; return true;
}

} // namespace a5
