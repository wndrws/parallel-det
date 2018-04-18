cmake_minimum_required(VERSION 3.10)
project(Det_WinThreads)

set(CMAKE_CXX_STANDARD 17)

add_executable(Det_WinThreads main.cpp catch.hpp det_maths.h det_maths.cpp det_parallel.hpp time_measurement.hpp)
add_executable(Det main.cpp catch.hpp det_maths.h det_maths.cpp det_parallel.hpp time_measurement.hpp)
target_compile_definitions(Det PRIVATE TESTS_ENABLED=0 PRIVATE PARALLELISM_ENABLED=0)
target_compile_definitions(Det_WinThreads PRIVATE TESTS_ENABLED=0 PRIVATE PARALLELISM_ENABLED=1)