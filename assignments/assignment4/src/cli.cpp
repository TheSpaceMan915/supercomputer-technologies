// Command-line parser for ping-pong benchmark options.
// Handles --warmup, --iters, --min-bytes, --max-bytes, --factor.
// Uses strtol for safe integer parsing; validates constraints.

#include "assignment4/cli.h"
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <cstring>

namespace {

// Safe integer parser: checks for overflow, valid endings, and range.
// Returns false if input is malformed or out of int range.
bool parse_int(const char* s, int& out) {
    if (!s || *s == '\0') return false;
    errno = 0;
    char* endp = 0;
    long v = std::strtol(s, &endp, 10);
    if (errno == ERANGE || endp == s || *endp != '\0') return false;
    if (v > INT_MAX || v < INT_MIN) return false;
    out = static_cast<int>(v);
    return true;
}

}

namespace assignment4 {

bool parse_cli(int argc, char** argv, Options& opt, std::string& err)
{
    int i = 1;
    while (i < argc) {
        const char* a = argv[i];
        if (!a) { ++i; continue; }

        if (0 == std::strcmp(a, "--warmup")) {
            if (i + 1 >= argc) { err = "missing value for --warmup"; return false; }
            int v; if (!parse_int(argv[i+1], v)) { err = "invalid --warmup"; return false; }
            if (v < 0) { err = "--warmup must be >= 0"; return false; }
            opt.warmup = v; i += 2; continue;
        }
        if (0 == std::strcmp(a, "--iters")) {
            if (i + 1 >= argc) { err = "missing value for --iters"; return false; }
            int v; if (!parse_int(argv[i+1], v)) { err = "invalid --iters"; return false; }
            if (v <= 0) { err = "--iters must be > 0"; return false; }
            opt.iters = v; i += 2; continue;
        }
        if (0 == std::strcmp(a, "--min-bytes")) {
            if (i + 1 >= argc) { err = "missing value for --min-bytes"; return false; }
            int v; if (!parse_int(argv[i+1], v)) { err = "invalid --min-bytes"; return false; }
            if (v <= 0) { err = "--min-bytes must be > 0"; return false; }
            opt.min_bytes = v; i += 2; continue;
        }
        if (0 == std::strcmp(a, "--max-bytes")) {
            if (i + 1 >= argc) { err = "missing value for --max-bytes"; return false; }
            int v; if (!parse_int(argv[i+1], v)) { err = "invalid --max-bytes"; return false; }
            if (v < 1) { err = "--max-bytes must be >= 1"; return false; }
            opt.max_bytes = v; i += 2; continue;
        }
        if (0 == std::strcmp(a, "--factor")) {
            if (i + 1 >= argc) { err = "missing value for --factor"; return false; }
            int v; if (!parse_int(argv[i+1], v)) { err = "invalid --factor"; return false; }
            if (v < 2) { err = "--factor must be >= 2"; return false; }
            opt.factor = v; i += 2; continue;
        }

        err = std::string("invalid option: ") + a;
        return false;
    }

    // Final cross-field validation
    if (opt.min_bytes > opt.max_bytes) { err = "min-bytes must be <= max-bytes"; return false; }
    return true;
}

} // namespace assignment4
