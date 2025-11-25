# assignment4 — MPI Ping-Pong Latency (C++98)

One-way latency benchmark between two MPI ranks (`rank 0` and `rank 1`).
Message size starts at **4 bytes** and doubles each step until **10 MiB**.
Rank 0 performs a timed ping-pong using `MPI_Wtime()` and reports latency
in microseconds as: `lat_us = (round_trip_seconds / (2 * iters)) * 1e6`.

## Build (Open MPI compatible)

```bash
cmake -S . -B build-a4
cmake --build build-a4
```

If you prefer wrapper compilers, set `CMAKE_CXX_COMPILER=mpic++` when configuring.

## Run (two ranks)

```bash
mpirun -np 2 ./build-a4/assignment4   --warmup 10 --iters 100 --min-bytes 4 --max-bytes 10485760 --factor 2
```

Sample output (rank 0 only):
```
[INFO] assignment4 start
[INFO] ranks=2 warmup=10 iters=100 min=4 max=10485760 factor=2
[INFO] mode=ping-pong
[INFO] size=4 B latency_us=...
...
[INFO] size=10485760 B latency_us=...
[INFO] assignment4 done
```

## Notes

* Uses the **MPI C API** from `<mpi.h>` (Open MPI 1.2.7 compatible).
* Uses `std::vector<char>` for payload; no raw `new[]`.
* Geometric growth: `S(k+1) = S(k) * factor` (inclusive up to `max-bytes`).
* Only rank 0 prints results; rank 1 stays quiet (errors excepted).
* Use CPU pinning / isolated cores to reduce noise for stable numbers.
