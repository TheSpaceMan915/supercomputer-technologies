
# supercomputer-technologies — CMake 3.8.2, C++98

A parent CMake project that aggregates several small, self‑contained C++98 assignments. 
Each child can be built **standalone** or via the **parent**. The code compiles on GCC/Clang/MSVC. 
OpenMP is used where noted (falls back to serial if not enabled). MPI is used for the ping‑pong benchmark.

## Requirements

- CMake **3.8.2+**
- A C++ compiler with **C++98** support (GCC/Clang/MSVC)
- Optional: **OpenMP 3.0** for `assignment3-task1` and `assignment3-task2`
- Required for `assignment4`: **MPI** (Open MPI 1.2.7 compatible). You may use `mpic++` or let CMake `find_package(MPI)`.

## Children overview

- **assignments/assignment1** — π via midpoint rule (serial), simple logger + Unity unit tests.
- **assignments/assignment2** — Dense N×N matrix multiply (serial), reports FLOPS/GFLOPS, Unity tests.
- **assignments/assignment3-task1** — π via midpoint rule **OpenMP** parallel reduction (falls back to serial if built without OpenMP). Logs threads, π, error, elapsed.
- **assignments/assignment3-task2** — Dense N×N matrix multiply **OpenMP** over rows of `C` (falls back to serial). Logs boundary elements, elapsed, FLOPS/GFLOPS.
- **assignments/assignment4** — **MPI Ping‑Pong** one‑way latency benchmark (message sizes grow from 4 B doubling up to 10 MiB). Rank 0 prints results.

All children use out‑of‑source builds and enable CTest; some ship a tiny vendored Unity for unit tests.

## Build — parent (all children)

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

This produces child executables under `build/assignments/<child>/<child>`.

## Build — standalone children

> Useful for focused iteration or when only one dependency (e.g., MPI) is installed.

**assignment1** (π midpoint, serial)
```bash
cmake -S assignments/assignment1 -B build-a1
cmake --build build-a1
./build-a1/assignment1 1000000
```

**assignment2** (matrix multiply, serial)
```bash
cmake -S assignments/assignment2 -B build-a2
cmake --build build-a2
./build-a2/assignment2 512
```

**assignment3-task1** (π midpoint, OpenMP; add `-fopenmp` or `/openmp` in your toolchain)
```bash
cmake -S assignments/assignment3-task1 -B build-a3t1
cmake --build build-a3t1
# Optional: control threads
export OMP_NUM_THREADS=8
./build-a3t1/assignment3-task1 1000000
```

**assignment3-task2** (matrix multiply, OpenMP)
```bash
cmake -S assignments/assignment3-task2 -B build-a3t2
cmake --build build-a3t2
export OMP_NUM_THREADS=8
./build-a3t2/assignment3-task2 512
```

**assignment4** (MPI Ping‑Pong; requires MPI runtime)
```bash
cmake -S assignments/assignment4 -B build-a4
cmake --build build-a4
mpirun -np 2 ./build-a4/assignment4 --warmup 10 --iters 100 --min-bytes 4 --max-bytes 10485760 --factor 2
```

## Notes & tips

- **Out‑of‑source only**: never configure in the source tree. Prefer `-B build-*` folders.
- **OpenMP**: If CMake can’t find imported targets on your platform, legacy variables are used (e.g., `${OpenMP_CXX_FLAGS}`).
- **MPI (assignment4)**: You can set `CMAKE_CXX_COMPILER=mpic++` at configure time, or rely on `find_package(MPI)`.
- **Windows**: Use a recent MSVC; for MPI, install MS‑MPI or another MPI and ensure `mpiexec` is on PATH.
- **Testing**: run `ctest --output-on-failure` in the chosen build directory. Unity is vendored for helper‑level tests.

## Example outputs (abridged)

- `assignment3-task1 1000000`:
```
[INFO] assignment3-task1 start
[INFO] n=1000000 threads=8
[INFO] pi=3.14159265 error=1.2e-06 elapsed_ms=42
[INFO] assignment3-task1 done
```

- `assignment3-task2 512`:
```
[INFO] assignment3-task2 start
[INFO] N=512
[INFO] mode=parallel threads=8
[INFO] C[0][0]=... C[0][N-1]=... C[N-1][0]=... C[N-1][N-1]=...
[INFO] elapsed_ms=123.45 flops=2.68e+08 gflops=2.17
[INFO] assignment3-task2 done
```

- `mpirun -np 2 assignment4 ...`:
```
[INFO] assignment4 start
[INFO] ranks=2 warmup=10 iters=100 min=4 max=10485760 factor=2
[INFO] mode=ping-pong
[INFO] size=4 B latency_us=...
...
[INFO] size=10485760 B latency_us=...
[INFO] assignment4 done
```

## House rules

- CMake 3.8.2, C++98. No C++11+ features in these assignments.
- Prefer `std::vector` for storage; no raw `new[]` in the user code paths.
- Keep warnings high (GCC/Clang: `-Wall -Wextra -Wpedantic`, MSVC: `/W4`).
