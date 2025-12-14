# assignment5 — Row-block MPI GEMM (C++98, Open MPI 1.2.7 compatible)

**Goal.** Multiply two dense \(N \times N\) matrices `A` and `B` in parallel using a simple **row-block** distribution.

- C++98, CMake 3.8.2
- MPI C API (`<mpi.h>`), MPI-1 friendly (`MPI_Bcast`, `MPI_Send/Recv`, `MPI_Barrier`, `MPI_Wtime`)

## Initialization
- `A[i][k] = i + 1`
- `B[k][j] = 1.0 / (j + 1)`

This yields a predictable result: \(C[i][j] = N\cdot\frac{i+1}{j+1}\).

## Build
```bash
cmake -S assignments/assignment5 -B build-a5 && cmake --build build-a5
```

## Run
```bash
mpirun -np 4 ./build-a5/assignment5 1024 --iters 3
```

Sample output (rank 0):
```
[INFO] assignment5 start
[INFO] N=1024 iters=3 ranks=4 dist=row-block
[INFO] C[0][0]=1024.00000000 C[0][1023]=1.00097752 C[1023][0]=1048576.00000000 C[1023][1023]=1024.00097752
[INFO] elapsed_ms=xxx.xxx flops=2.14748e+09 gflops=yyy.yyy
[INFO] assignment5 done
```
