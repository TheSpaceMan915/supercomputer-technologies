# Overview

This program measures one-way latency via an MPI ping-pong between two ranks:

* Rank 0: send → receive (timed round-trip).
* Rank 1: receive → send (mirror).
* One-way latency = round-trip / 2 / iterations.

Sizes start at 4 bytes and grow geometrically by `factor` until `max-bytes`.
Warmup exchanges are untimed to stabilize caches and message paths.
