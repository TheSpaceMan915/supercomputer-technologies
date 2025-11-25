# assignment4: MPI ping-pong latency benchmark (C++98)
cmake_minimum_required(VERSION 3.8.2)

project(assignment4 VERSION 0.1.0 LANGUAGES C CXX)

# Enforce out-of-source builds to keep source tree clean
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  message(FATAL_ERROR "Out-of-source builds only. Try: cmake -S . -B build-a4")
endif()

# C++98 standard required for portability (no C++11+ features)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Helper function: apply strict warnings to a target
function(a4_warnings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

# MPI is required for point-to-point communication
find_package(MPI REQUIRED)

# Core library: CLI, logger, size generator, ping-pong benchmark logic
add_library(assignment4_core
  src/cli.cpp
  src/logger.cpp
  src/sizes.cpp
  src/pingpong.cpp
)

# Expose public headers to consumers and tests
target_include_directories(assignment4_core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

a4_warnings(assignment4_core)

# Link MPI to core library (supports both modern and legacy FindMPI results)
if(TARGET MPI::MPI_CXX)
  target_link_libraries(assignment4_core PRIVATE MPI::MPI_CXX)
else()
  include_directories(${MPI_CXX_INCLUDE_PATH})
  target_compile_options(assignment4_core PRIVATE ${MPI_CXX_COMPILE_FLAGS})
  target_link_libraries(assignment4_core PRIVATE ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})
endif()

# Main executable: runs the benchmark (use mpiexec/mpirun to launch)
add_executable(assignment4 src/main.cpp)
target_link_libraries(assignment4 PRIVATE assignment4_core)
a4_warnings(assignment4)

# Link MPI to main executable
if(TARGET MPI::MPI_CXX)
  target_link_libraries(assignment4 PRIVATE MPI::MPI_CXX)
else()
  target_compile_options(assignment4 PRIVATE ${MPI_CXX_COMPILE_FLAGS})
  target_link_libraries(assignment4 PRIVATE ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})
endif()

# Testing setup
include(CTest)
enable_testing()

# Unity test framework (vendored for unit tests)
add_library(assignment4_unity STATIC tests/vendor/unity/unity.c)
target_include_directories(assignment4_unity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tests/vendor/unity)
a4_warnings(assignment4_unity)

# Unit tests: validate CLI parsing and size generation
add_executable(assignment4_tests tests/unit_tests.cpp)
target_link_libraries(assignment4_tests PRIVATE assignment4_core assignment4_unity)
a4_warnings(assignment4_tests)

add_test(NAME assignment4_unit_tests COMMAND assignment4_tests)

# Smoke test: run ping-pong with 2 MPI ranks to verify basic functionality
if(MPIEXEC)
  add_test(NAME assignment4_mpi_smoke
           COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 2
                   $<TARGET_FILE:assignment4>
                   --warmup 4 --iters 16 --min-bytes 4 --max-bytes 4096 --factor 2)
endif()
