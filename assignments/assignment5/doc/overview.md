# Overview

Row-block distributed dense GEMM:

1. Rank 0 initializes `B`, broadcasts it to all ranks.
2. Each rank computes its local rows of `C = A·B` using the classic triple loop.
3. Only four boundary entries of `C` are collected to rank 0 for logging.
