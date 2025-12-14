#ifndef ASSIGNMENT5_MATRIX_H
#define ASSIGNMENT5_MATRIX_H

#include <vector>

namespace a5 {

void init_B(std::vector<double>& B, int N);

void compute_local_rows(
    int N,
    int row_offset,
    int row_count,
    const std::vector<double>& B,
    double* c00,
    double* c0N1,
    double* cN10,
    double* cN1N1);

bool exceeds_memory_budget_for_B(int N, unsigned long long threshold_bytes);

} // namespace a5

#endif
