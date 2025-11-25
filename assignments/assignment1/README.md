# assignment1 — π via the Midpoint Rule (C++98)

Approximates π using the midpoint Riemann sum:
\[
\pi \approx \frac{1}{n}\sum_{i=1}^{n}\frac{4}{1+\left(\frac{i-0.5}{n}\right)^2}
\]

- CLI: `assignment1 <n>` where `n` is a positive integer.
- Logs: start → parsed `n` → π value → absolute error vs `M_PI` → elapsed CPU ms → done.
- C++98, portable across GCC/Clang/MSVC. Uses `std::clock()` for CPU-time.

## Build (standalone)
```bash
cmake -S . -B build-a1
cmake --build build-a1
ctest --test-dir build-a1 --output-on-failure
./build-a1/assignment1 100000
```
