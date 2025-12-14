cmake_minimum_required(VERSION 3.8.2)
project(assignment5 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(MPI REQUIRED)

add_library(assignment5_core
  src/cli.cpp
  src/logger.cpp
  src/dist.cpp
  src/matrix.cpp
)
target_include_directories(assignment5_core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if (TARGET MPI::MPI_CXX)
  target_link_libraries(assignment5_core PUBLIC MPI::MPI_CXX)
else()
  include_directories(${MPI_CXX_INCLUDE_PATH})
  target_compile_options(assignment5_core PUBLIC ${MPI_CXX_COMPILE_FLAGS})
  target_link_libraries(assignment5_core PUBLIC ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})
endif()

add_executable(assignment5 src/main.cpp)
target_link_libraries(assignment5 PRIVATE assignment5_core)

enable_testing()

add_library(assignment5_unity STATIC tests/vendor/unity/unity.c)
target_include_directories(assignment5_unity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tests/vendor/unity)

add_executable(assignment5_tests tests/unit_tests.cpp)
target_link_libraries(assignment5_tests PRIVATE assignment5_core assignment5_unity)
add_test(NAME assignment5_unit_tests COMMAND assignment5_tests)

include(CTest)
if (MPIEXEC)
  add_test(NAME assignment5_mpi_smoke
    COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 4
            $<TARGET_FILE:assignment5> 512 --iters 1)
endif()
