cmake_minimum_required(VERSION 3.10)
project(Det_WinThreads)

set(CMAKE_CXX_STANDARD 17)

find_library(MPI_LIBRARY mpi PATHS ENV MPI_LIB64)

add_executable(Det_WinThreads main.cpp catch.hpp det_maths.h det_maths.cpp det_parallel.hpp time_measurement.hpp)
add_executable(Det main.cpp catch.hpp det_maths.h det_maths.cpp det_parallel.hpp time_measurement.hpp)
target_compile_definitions(Det PRIVATE TESTS_ENABLED=0 PRIVATE PARALLELISM_ENABLED=0)
target_compile_definitions(Det_WinThreads PRIVATE TESTS_ENABLED=0 PRIVATE PARALLELISM_ENABLED=1)

add_executable(Det_MPI mpimain.cpp det_maths.h det_maths.cpp det_parallel.hpp time_measurement.hpp MPIEnv.cpp MPIEnv.h)
target_include_directories(Det_MPI PRIVATE $ENV{MPI_INC})
target_link_libraries(Det_MPI ${MPI_LIBRARY})