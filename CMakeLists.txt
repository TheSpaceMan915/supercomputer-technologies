# Parent CMakeLists.txt - orchestrates all child projects
# Sets C++98 standard globally and enables out-of-source builds only.
# Aggregates assignments/assignment1 and assignments/assignment2 as subdirectories and enables CTest for testing.

cmake_minimum_required(VERSION 3.8.2)
project(supercomputer_technologies LANGUAGES C CXX)

# Enforce out-of-source builds to keep source tree clean
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "Out-of-source builds only. Use: cmake -S . -B build")
endif()

# Enforce C++98 standard globally (no C++11+ features allowed)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable testing framework (CTest) for all child projects
include(CTest)

# Add the assignment1 child project (π approximation via midpoint rule)
add_subdirectory(assignments/assignment1)

# Add the assignment2 child project (matrix multiplication)
add_subdirectory(assignments/assignment2)

# Add the assignment3-task1 child project (π via midpoint with OpenMP reduction)
add_subdirectory(assignments/assignment3-task1)

# Add the assignment3-task2 child project (matrix multiplication with OpenMP)
add_subdirectory(assignments/assignment3-task2)

# Add the assignment4 child project (MPI Ping-Pong)
add_subdirectory(assignments/assignment4)

# Add the assignment5 child project (MPI GEMM algorithm)
add_subdirectory(assignments/assignment5)