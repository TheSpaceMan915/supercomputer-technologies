// pi.cpp - Implementation of π approximation via midpoint rule
// Uses numerical integration of f(x) = 4/(1+x²) over [0,1] which equals π.
// Midpoint rule: sample f at x = (i-0.5)/n for each subinterval.

#include "assignment1/pi.h"

namespace assignment1 {

// Approximate π using the midpoint rule with n subintervals.
// The integral of 4/(1+x²) from 0 to 1 equals π (arctangent identity).
// We partition [0,1] into n equal-width intervals and evaluate f at the midpoint
// of each: x_i = (i - 0.5)/n for i=1..n, then sum and multiply by width 1/n.
// Returns: π estimate, or 0.0 if n <= 0
// Complexity: O(n) time, O(1) space
double approximate_pi(int n)
{
    if (n <= 0) {
        return 0.0;
    }
    const double inv_n = 1.0 / static_cast<double>(n);  // Subinterval width
    double sum = 0.0;
    for (int i = 1; i <= n; ++i) {
        // Midpoint of i-th subinterval: (i - 0.5)/n
        const double x = (static_cast<double>(i) - 0.5) * inv_n;
        // f(x) = 4/(1 + x²)
        sum += 4.0 / (1.0 + x * x);
    }
    // Multiply sum by subinterval width to get integral approximation
    return sum * inv_n;
}

} // namespace assignment1
