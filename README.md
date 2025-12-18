# supercomputer-technologies

A collection of small, focused C++98 assignments that explore performance‑oriented programming across single‑node (serial/OpenMP) and distributed (MPI) settings. Every child project is standalone (can be configured on its own) **and** can be built together from the parent with modern CMake 3.8.2. The code follows a conservative C++98 subset so it compiles with GCC/Clang/MSVC across Linux, macOS, and Windows (MSYS2/Cygwin).

---

## What’s inside

- **assignments/assignment1** — π via midpoint rule (serial), with logging & a tiny Unity‑style test harness.  
- **assignments/assignment2** — dense `C = A·B` (double) with a naïve triple loop, contiguous 1‑D storage; OpenMP optional in the child.  
- **assignments/assignment3-task1** — π via midpoint rule **parallelized with OpenMP 3.0** (`reduction(+:sum)`).  
- **assignments/assignment3-task2** — dense matrix multiply **parallelized with OpenMP 3.0** over the outer loop(s).  
- **assignments/assignment4** — **MPI Ping‑Pong** one‑way latency benchmark (two ranks; sizes 4 B → 10 MiB).  
- **assignments/assignment5** — **MPI row‑block matrix multiply** (broadcast B, each rank computes its rows of C).

> Each child ships: `CMakeLists.txt`, headers in `include/<child>/`, sources in `src/`, tests in `tests/` (Unity vendored), and brief docs in `doc/` + `README.md`.

---

## Requirements

- **CMake** ≥ 3.8.2
- A C++98‑capable compiler (GCC, Clang, or MSVC)
- **OpenMP 3.0** (for `assignment3-task1`, `assignment3-task2`)
- **MPI (Open MPI 1.x/4.x/5.x or compatible)** (for `assignment4`, `assignment5`)

### Quick install hints (non‑exhaustive)

- **macOS (Homebrew)**  
  ```bash
  brew install cmake llvm open-mpi
  # If using AppleClang, OpenMP is not enabled; use Homebrew clang:
  export CC=$(brew --prefix)/opt/llvm/bin/clang
  export CXX=$(brew --prefix)/opt/llvm/bin/clang++
  ```

- **Ubuntu/Debian**  
  ```bash
  sudo apt-get update
  sudo apt-get install -y build-essential cmake libomp-dev libopenmpi-dev openmpi-bin
  ```

- **Windows (MSYS2 MinGW‑w64)**  
  ```bash
  pacman -S --needed mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake
  # OpenMP: included in GCC; MPI: prefer Cygwin OpenMPI or WSL OpenMPI for MPI assignments
  ```

- **Windows (Cygwin)**  
  Install `gcc-g++`, `cmake`, and `openmpi` packages; use `mpic++`, `mpirun` from Cygwin.

> On all platforms ensure `mpicc/mpic++/mpirun` (MPI) or an OpenMP‑enabled compiler are in `PATH` when building MPI/OpenMP assignments.

---

## Parent build (aggregate all children)

Out‑of‑source build is required/recommended.

```bash
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
```

This configures, builds, and runs tests for every child added via `add_subdirectory(assignments/<child>)`.

---

## Build a single child (standalone)

You can work with any child directly:

```bash
# Example: assignment1 (serial π)
cmake -S assignments/assignment1 -B build-a1
cmake --build build-a1 -j
ctest --test-dir build-a1 --output-on-failure
```

```bash
# Example: assignment2 (naïve GEMM; OpenMP optional)
cmake -S assignments/assignment2 -B build-a2
cmake --build build-a2 -j
./build-a2/assignment2 512
```

```bash
# Example: assignment3-task1 (OpenMP)
cmake -S assignments/assignment3-task1 -B build-a3t1
cmake --build build-a3t1 -j
# At run time (Linux/macOS):
OMP_NUM_THREADS=8 ./build-a3t1/assignment3-task1 1000000
```

```bash
# Example: assignment3-task2 (OpenMP)
cmake -S assignments/assignment3-task2 -B build-a3t2
cmake --build build-a3t2 -j
OMP_NUM_THREADS=8 ./build-a3t2/assignment3-task2 512
```

```bash
# Example: assignment4 (MPI Ping-Pong)
cmake -S assignments/assignment4 -B build-a4
cmake --build build-a4 -j
mpirun -np 2 ./build-a4/assignment4 --warmup 10 --iters 100 --min-bytes 4 --max-bytes 10485760 --factor 2
```

```bash
# Example: assignment5 (MPI row-block GEMM)
cmake -S assignments/assignment5 -B build-a5
cmake --build build-a5 -j
mpirun -np 4 ./build-a5/assignment5 1024 --iters 3
```

> On macOS (Homebrew LLVM) export `CC/CXX` to Homebrew clang to enable OpenMP; on Windows/Cygwin ensure you run inside the **Cygwin** shell so `mpic++`/`mpirun` are available.

---

## CLion notes (optional)

- Use **CMake profile** with your desired generator (`Ninja` or `Unix Makefiles`).
- For OpenMP children, set **Environment**: `OMP_NUM_THREADS=8` in the run config.
- For MPI children, create a **Shell Script** run configuration that invokes `mpirun` with the built executable path.
- If OpenMPI is via Homebrew, add its `bin/` to `PATH` in CLion’s run configuration environment.

---

## Troubleshooting

- **`mpi.h: No such file or directory`**  
  Ensure MPI headers are installed and the right compiler is used (`mpic++`) or CMake locates MPI (`find_package(MPI)`).
- **`mpirun: command not found` or permission issues**  
  Add MPI `bin/` to `PATH`. On macOS, avoid shell‑variable literals (e.g., `"$PROJECT_DIR$"`); pass absolute paths.
- **OpenMP has no effect**  
  Confirm you compiled with OpenMP flags (`-fopenmp` with GCC/Clang from Homebrew; `/openmp` on MSVC). AppleClang does not ship OpenMP by default.

---

## Contributing

Small, focused PRs are welcome: build fixes, doc clarifications, new unit tests, or portability improvements. Keep code in C++98 and prefer portable CMake (3.8.2).

---

## License

If this repository includes a `LICENSE` file, its terms apply. Otherwise, treat the code as “all rights reserved” until a license is added.
