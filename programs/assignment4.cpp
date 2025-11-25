// Build on Linux/macOS, OpenMPI:
//   mpic++ -std=c++98 -O2 -Wall -Wextra -Wpedantic -o assignment4 assignment4.cpp
//
// Build on Windows (Cygwin with OpenMPI):
//   mpic++ -std=c++98 -O2 -Wall -Wextra -Wpedantic -o assignment4 assignment4.cpp
//
// Run the program (Linux/macOS/Windows, MPI required):
//   mpirun -np 2 ./assignment4 100 10 4 10485760 2

#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Helper function to log info
void log_info(const char* msg) {
    std::cout << "[INFO] " << msg << std::endl;
}

// Helper function to log error messages
void log_error(const char* msg) {
    std::cerr << "[ERROR] " << msg << std::endl;
}

// Function to parse command-line arguments
bool parse_n(int argc, char** argv, int& n) {
    if (argc < 2) {
        log_error("Missing arguments");
        return false;
    }
    
    n = std::atoi(argv[1]);
    if (n <= 0) {
        log_error("Invalid N value");
        return false;
    }
    return true;
}

// Function to initialize message buffers
void init_buffers(std::vector<char>& buffer, int size) {
    buffer.resize(size);
    for (int i = 0; i < size; ++i) {
        buffer[i] = static_cast<char>(i % 256);  // Fill with some pattern
    }
}

// Function to benchmark latency (Ping-Pong pattern)
void benchmark_latency(int rank, int N, int iters, int warmup) {
    // Allocate message buffers
    std::vector<char> send_buffer, recv_buffer;
    init_buffers(send_buffer, N);
    init_buffers(recv_buffer, N);

    // Synchronize the processes
    MPI_Barrier(MPI_COMM_WORLD);

    // Warmup iterations (not timed)
    for (int i = 0; i < warmup; ++i) {
        if (rank == 0) {
            MPI_Send(send_buffer.data(), N, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(recv_buffer.data(), N, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(recv_buffer.data(), N, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(send_buffer.data(), N, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    }

    // Measure the time for actual iterations
    double start_time = MPI_Wtime();
    for (int i = 0; i < iters; ++i) {
        if (rank == 0) {
            MPI_Send(send_buffer.data(), N, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(recv_buffer.data(), N, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(recv_buffer.data(), N, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(send_buffer.data(), N, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    }
    double end_time = MPI_Wtime();

    // Calculate round-trip time and latency
    double round_trip_time = end_time - start_time;
    double latency = (round_trip_time / (2 * iters)) * 1e6;  // in microseconds

    // Log the results
    if (rank == 0) {
        std::cout << "[INFO] size=" << N << " B latency_us=" << latency << std::endl;
    }
}

int main(int argc, char** argv) {
    // Initialize MPI
    MPI_Init(&argc, &argv);

    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the total number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Ensure exactly 2 processes
    if (world_size != 2) {
        log_error("World size must be 2");
        MPI_Finalize();
        return 1;
    }

    // Parse command-line arguments
    int iters = 100, warmup = 10, min_size = 4, max_size = 10485760, factor = 2;
    if (!parse_n(argc, argv, min_size)) {
        MPI_Finalize();
        return 1;
    }
    if (argc > 2) iters = std::atoi(argv[2]);
    if (argc > 3) warmup = std::atoi(argv[3]);
    if (argc > 4) max_size = std::atoi(argv[4]);
    if (argc > 5) factor = std::atoi(argv[5]);

    log_info("assignment4 start");
    std::cout << "[INFO] warmup=" << warmup << " iters=" << iters << " min=" << min_size << " max=" << max_size << " factor=" << factor << std::endl;

    // Run the benchmark for message sizes from min_size to max_size with the specified factor
    int size = min_size;
    while (size <= max_size) {
        benchmark_latency(rank, size, iters, warmup);
        size *= factor;
    }

    log_info("assignment4 done");

    // Finalize MPI
    MPI_Finalize();
    return 0;
}
