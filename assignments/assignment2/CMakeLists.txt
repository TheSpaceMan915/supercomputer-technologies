# assignment2 CMakeLists.txt — Matrix multiplication benchmark (C++98)
# Defines targets: assignment2_core (static lib), assignment2 (CLI binary),
# assignment2_unity (Unity test framework), assignment2_tests (test binary).
# Enforces C++98, out-of-source builds, and strict compiler warnings.

cmake_minimum_required(VERSION 3.8.2)
project(assignment2 VERSION 0.1.0 LANGUAGES C CXX)

# Enforce out-of-source builds to keep source tree clean
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  message(FATAL_ERROR "Out-of-source builds only. Try: cmake -S . -B build-a2")
endif()

# Force C++98 standard globally (no C++11+ features)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Helper function to apply consistent warning flags across targets
function(set_common_warnings tgt)
  if(MSVC)
    target_compile_options(${tgt} PRIVATE /W4)
  else()
    target_compile_options(${tgt} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

# assignment2_core: static library with matrix and logger implementations
add_library(assignment2_core STATIC
  src/matrix.cpp
  src/logger.cpp
)

# Expose include/ for public headers; src/ for private includes
target_include_directories(assignment2_core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_common_warnings(assignment2_core)

# assignment2: main CLI executable that links against core library
add_executable(assignment2 src/main.cpp)
target_link_libraries(assignment2 PRIVATE assignment2_core)
set_common_warnings(assignment2)

# Enable CTest support for this child project
include(CTest)
enable_testing()

# assignment2_unity: vendored Unity test framework (C code)
add_library(assignment2_unity STATIC tests/vendor/unity/unity.c)
target_include_directories(assignment2_unity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tests/vendor/unity)
set_common_warnings(assignment2_unity)

# assignment2_tests: test binary linking core + Unity for unit tests
add_executable(assignment2_tests tests/unit_tests.cpp)
target_link_libraries(assignment2_tests PRIVATE assignment2_core assignment2_unity)
target_include_directories(assignment2_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/tests
)
set_common_warnings(assignment2_tests)

# Register test with CTest so `ctest` runs assignment2_tests
add_test(NAME assignment2_tests COMMAND assignment2_tests)
