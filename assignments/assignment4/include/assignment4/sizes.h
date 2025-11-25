// Geometric message-size sequence generator for bandwidth/latency sweeps.
// Generates [min_bytes, min*factor, min*factor^2, ...] up to max_bytes inclusive.
// Depends on: <vector>.

#ifndef ASSIGNMENT4_SIZES_H
#define ASSIGNMENT4_SIZES_H

#include <vector>

namespace assignment4 {

// Builds geometric sequence: S(k+1) = S(k) * factor, stopping at max_bytes.
// Returns false if constraints violated (min <= 0, max <= 0, factor < 2, min > max).
// Uses long long internally to detect overflow before storing int.
bool make_sizes(int min_bytes, int max_bytes, int factor, std::vector<int>& out);

} // namespace assignment4

#endif
