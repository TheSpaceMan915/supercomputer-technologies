# assignment2 — Matrix Multiplication (C++98)

Classic triple-loop `C = A · B` with initialization:
- `A[i][j] = i + 1`
- `B[i][j] = 1.0 / (j + 1)`

This yields an easy closed form: `C[i][j] = N * (i + 1) / (j + 1)`.

## CLI
```
assignment2 <N>
```

## Logs
- start + `N`
- boundary elements: `C[0][0]`, `C[0][N-1]`, `C[N-1][0]`, `C[N-1][N-1]`
- `elapsed_ms` (CPU time via `std::clock()`), `flops = 2*N^3`, `gflops`
- end banner

## Build (standalone)
```bash
cmake -S . -B build-a2
cmake --build build-a2
ctest --test-dir build-a2 --output-on-failure
./build-a2/assignment2 512
```
