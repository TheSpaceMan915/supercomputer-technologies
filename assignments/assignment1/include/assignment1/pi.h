// pi.h - π approximation using the midpoint rule (numerical integration)
// Provides approximate_pi(n) which integrates 4/(1+x²) over [0,1] to estimate π.
// Higher n yields better accuracy but slower runtime (O(n) complexity).

#ifndef ASSIGNMENT1_PI_H
#define ASSIGNMENT1_PI_H

namespace assignment1 {

// Approximate π using the midpoint rule with n subintervals.
// Formula: π ≈ (1/n) * Σ(i=1 to n) 4/(1 + ((i-0.5)/n)²)
// Parameters:
//   n - Number of subintervals (larger n → better precision, O(n) time)
// Returns:
//   Estimated value of π, or 0.0 if n <= 0
// Note: Accuracy improves as O(1/n²); for n=100000, error is typically < 1e-5
double approximate_pi(int n);

} // namespace assignment1

#endif // ASSIGNMENT1_PI_H
