// pi.cpp — Pi approximation via midpoint rule with optional OpenMP parallelization
// Uses the integral identity: ∫[0,1] 4/(1+x²) dx = 4*arctan(1) = π
// The midpoint rule approximates the integral by summing function values at interval midpoints.

#include "assignment3_task1/pi.h"

#ifdef _OPENMP
#  include <omp.h>
#endif

namespace assignment3_task1 {

// Serial implementation of midpoint rule for pi approximation
double approximate_pi_serial(int n) {
  // Edge case: invalid or zero intervals
  if (n <= 0) {
    return 0.0;
  }

  const double interval_width = 1.0 / static_cast<double>(n);
  double sum = 0.0;

  // Sum f(x_i) where x_i is the midpoint of each subinterval
  for (int i = 0; i < n; ++i) {
    const double midpoint = (static_cast<double>(i) + 0.5) * interval_width;
    sum += 4.0 / (1.0 + midpoint * midpoint);
  }

  return interval_width * sum;
}

// Parallel implementation using OpenMP reduction (if available)
double approximate_pi_parallel(int n) {
  // Edge case: invalid or zero intervals
  if (n <= 0) {
    return 0.0;
  }

  const double interval_width = 1.0 / static_cast<double>(n);

#ifndef _OPENMP
  // Fall back to serial if OpenMP is not available
  return approximate_pi_serial(n);
#else
  double sum = 0.0;

  // OpenMP parallel loop with reduction to sum partial results
  // Static scheduling for predictable load distribution
  #pragma omp parallel for reduction(+:sum) schedule(static)
  for (int i = 0; i < n; ++i) {
    const double midpoint = (static_cast<double>(i) + 0.5) * interval_width;
    sum += 4.0 / (1.0 + midpoint * midpoint);
  }

  return interval_width * sum;
#endif
}

}  // namespace assignment3_task1
