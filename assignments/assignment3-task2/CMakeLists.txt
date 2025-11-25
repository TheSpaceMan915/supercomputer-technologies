# assignment3-task2 CMake configuration
# Builds matrix multiplication library and application with OpenMP support.
cmake_minimum_required(VERSION 3.8.2)

project(assignment3_task2 VERSION 0.1.0 LANGUAGES C CXX)

# Enforce out-of-source builds for clean separation
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
    message(FATAL_ERROR "Out-of-source builds only. Try: cmake -S . -B build-a3t2")
endif()

# Target C++98 for compatibility with older systems
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Apply strict warnings to catch potential issues
function(assignment3_task2_set_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# Discover OpenMP (optional dependency for parallel execution)
find_package(OpenMP)

# Check for modern CMake target-based OpenMP support
set(ASSIGNMENT3_TASK2_HAS_OMP_TARGET FALSE)
if(TARGET OpenMP::OpenMP_CXX)
    set(ASSIGNMENT3_TASK2_HAS_OMP_TARGET TRUE)
endif()

# Core library: matrix operations with OpenMP parallelization
add_library(assignment3_task2_core
    src/matrix.cpp
    src/logger.cpp
)

target_include_directories(assignment3_task2_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

assignment3_task2_set_warnings(assignment3_task2_core)

# Link OpenMP to core library (if available)
if(ASSIGNMENT3_TASK2_HAS_OMP_TARGET)
    target_link_libraries(assignment3_task2_core PUBLIC OpenMP::OpenMP_CXX)
elseif(OpenMP_CXX_FOUND)
    target_compile_options(assignment3_task2_core PUBLIC ${OpenMP_CXX_FLAGS})
    target_link_libraries(assignment3_task2_core PUBLIC ${OpenMP_CXX_LIBRARIES})
endif()

# Executable: command-line driver for matrix multiplication benchmark
add_executable(assignment3-task2
    src/main.cpp
)

target_link_libraries(assignment3-task2
    PRIVATE
        assignment3_task2_core
)

assignment3_task2_set_warnings(assignment3-task2)

# Link OpenMP to executable (needed for omp_get_max_threads, omp_get_wtime)
if(ASSIGNMENT3_TASK2_HAS_OMP_TARGET)
    target_link_libraries(assignment3-task2 PRIVATE OpenMP::OpenMP_CXX)
elseif(OpenMP_CXX_FOUND)
    target_compile_options(assignment3-task2 PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_libraries(assignment3-task2 PRIVATE ${OpenMP_CXX_LIBRARIES})
endif()

# Enable CTest for running tests via 'ctest'
include(CTest)
enable_testing()

# Unity testing framework (vendored C library)
add_library(assignment3_task2_unity STATIC
    tests/vendor/unity/unity.c
)
target_include_directories(assignment3_task2_unity
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/vendor/unity
)
assignment3_task2_set_warnings(assignment3_task2_unity)

# Test executable: validates serial and parallel implementations
add_executable(assignment3_task2_tests
    tests/unit_tests.cpp
)

target_link_libraries(assignment3_task2_tests
    PRIVATE
        assignment3_task2_core
        assignment3_task2_unity
)

target_include_directories(assignment3_task2_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

assignment3_task2_set_warnings(assignment3_task2_tests)

# Link OpenMP to tests (needed for parallel implementation)
if(ASSIGNMENT3_TASK2_HAS_OMP_TARGET)
    target_link_libraries(assignment3_task2_tests PRIVATE OpenMP::OpenMP_CXX)
elseif(OpenMP_CXX_FOUND)
    target_compile_options(assignment3_task2_tests PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_libraries(assignment3_task2_tests PRIVATE ${OpenMP_CXX_LIBRARIES})
endif()

# Register test with CTest
add_test(NAME assignment3_task2_tests COMMAND assignment3_task2_tests)
