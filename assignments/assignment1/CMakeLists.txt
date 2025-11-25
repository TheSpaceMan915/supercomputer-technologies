# assignment1 CMakeLists.txt - π approximation child project
# Builds a library (assignment1_core), a console executable (assignment1),
# and Unity-based unit tests (assignment1_tests). Enforces C++98 and strict warnings.

cmake_minimum_required(VERSION 3.8.2)
project(assignment1 VERSION 0.1.0 LANGUAGES C CXX)

# Enforce out-of-source builds for this child project as well
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  message(FATAL_ERROR "Out-of-source builds only. Use: cmake -S . -B build-assignment1")
endif()

# Enforce C++98 standard (matches parent but can be built standalone)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Helper function to set common compiler warnings (MSVC /W4, GCC/Clang -Wall -Wextra -Wpedantic)
function(set_common_warnings tgt)
  if(MSVC)
    target_compile_options(${tgt} PRIVATE /W4)
  else()
    target_compile_options(${tgt} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

# Core library: π approximation and logging logic
# Exposes public headers under include/ for reuse in main executable and tests
add_library(assignment1_core
  src/pi.cpp
  src/logger.cpp
)
target_include_directories(assignment1_core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set_common_warnings(assignment1_core)

# Main console executable: parses command-line n, calls approximate_pi, logs results
add_executable(assignment1 src/main.cpp)
target_link_libraries(assignment1 PRIVATE assignment1_core)
set_common_warnings(assignment1)

# Testing: build Unity framework and unit tests if BUILD_TESTING is enabled
include(CTest)
if(BUILD_TESTING)
  # Unity framework (C): vendored under tests/vendor/unity
  add_library(assignment1_unity STATIC tests/vendor/unity/unity.c)
  target_include_directories(assignment1_unity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tests/vendor/unity)
  set_common_warnings(assignment1_unity)

  # Unit tests executable: wraps Unity (C) from C++ via extern "C"
  add_executable(assignment1_tests tests/unit_tests.cpp)
  target_link_libraries(assignment1_tests PRIVATE assignment1_core assignment1_unity)
  target_include_directories(assignment1_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
  )
  set_common_warnings(assignment1_tests)

  # Register test with CTest (run via: ctest --test-dir build)
  add_test(NAME assignment1_tests COMMAND assignment1_tests)
endif()
