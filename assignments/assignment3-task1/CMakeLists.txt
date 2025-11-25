cmake_minimum_required(VERSION 3.8.2)
project(assignment3_task1 VERSION 0.1.0 LANGUAGES C CXX)

# Enforce out-of-source builds to keep source tree clean
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  message(FATAL_ERROR "Out-of-source builds only. Try: cmake -S . -B build-a3")
endif()

# Use C++98 standard (required for cross-platform compatibility)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Helper function to apply common warnings to targets
function(set_common_warnings tgt)
  if(MSVC)
    target_compile_options(${tgt} PRIVATE /W4)
  else()
    target_compile_options(${tgt} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

# Find OpenMP for parallel pi computation (optional, graceful fallback)
find_package(OpenMP)

# Static library with core logic (pi approximation, logging)
add_library(assignment3_task1_core STATIC src/pi.cpp src/logger.cpp)
target_include_directories(assignment3_task1_core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set_common_warnings(assignment3_task1_core)

# Link OpenMP to core library if available
if(TARGET OpenMP::OpenMP_CXX)
  target_link_libraries(assignment3_task1_core PUBLIC OpenMP::OpenMP_CXX)
elseif(OpenMP_CXX_FOUND)
  target_compile_options(assignment3_task1_core PUBLIC ${OpenMP_CXX_FLAGS})
  target_link_libraries(assignment3_task1_core PUBLIC ${OpenMP_CXX_LIBRARIES})
endif()

# Main executable
add_executable(assignment3-task1 src/main.cpp)
target_link_libraries(assignment3-task1 PRIVATE assignment3_task1_core)
set_common_warnings(assignment3-task1)

# Link OpenMP to executable if available
if(TARGET OpenMP::OpenMP_CXX)
  target_link_libraries(assignment3-task1 PRIVATE OpenMP::OpenMP_CXX)
elseif(OpenMP_CXX_FOUND)
  target_compile_options(assignment3-task1 PRIVATE ${OpenMP_CXX_FLAGS})
  target_link_libraries(assignment3-task1 PRIVATE ${OpenMP_CXX_LIBRARIES})
endif()

# Enable testing with CTest
include(CTest)
enable_testing()

# Unity test framework (vendored C library)
add_library(assignment3_task1_unity STATIC tests/vendor/unity/unity.c)
target_include_directories(assignment3_task1_unity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tests/vendor/unity)
set_common_warnings(assignment3_task1_unity)

# Unit test executable
add_executable(assignment3_task1_tests tests/unit_tests.cpp)
target_link_libraries(assignment3_task1_tests PRIVATE assignment3_task1_core assignment3_task1_unity)
target_include_directories(assignment3_task1_tests
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)
set_common_warnings(assignment3_task1_tests)

# Register test with CTest
add_test(NAME assignment3_task1_tests COMMAND assignment3_task1_tests)
