// pi.h — Pi approximation using the midpoint rule
// Computes pi via numerical integration: ∫[0,1] 4/(1+x²) dx = π
// Provides both serial and OpenMP-parallel implementations.

#ifndef ASSIGNMENT3_TASK1_PI_H
#define ASSIGNMENT3_TASK1_PI_H

namespace assignment3_task1 {

// Approximate pi using midpoint rule with n subintervals (serial).
// Returns 0.0 if n <= 0. Error decreases as O(1/n²).
double approximate_pi_serial(int n);

// Approximate pi using midpoint rule with n subintervals (OpenMP parallel if available).
// Falls back to serial if OpenMP is not enabled. Returns 0.0 if n <= 0.
double approximate_pi_parallel(int n);

}  // namespace assignment3_task1

#endif  // ASSIGNMENT3_TASK1_PI_H
