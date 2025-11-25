// Geometric message-size generator: [min, min*factor, min*factor^2, ...] <= max.
// Uses long long arithmetic to detect overflow before storing int values.

#include "assignment4/sizes.h"

namespace assignment4 {

bool make_sizes(int min_bytes, int max_bytes, int factor, std::vector<int>& out)
{
    out.clear();
    if (min_bytes <= 0 || max_bytes <= 0 || factor < 2) return false;
    if (min_bytes > max_bytes) return false;

    long long s = min_bytes;
    while (s <= max_bytes) {
        out.push_back(static_cast<int>(s));
        long long next = s * static_cast<long long>(factor);
        // Overflow guard: if multiplication wraps, stop
        if (next <= s) break;
        s = next;
    }
    return true;
}

} // namespace assignment4
