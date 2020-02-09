cmake_minimum_required(VERSION 3.0)
project(mcfile)

include(CheckCCompilerFlag)

include_directories(. ../include  ext/doctest)
add_definitions(-std=c++17)
add_executable(mcfile-test main.cpp Path.hpp Point3D.hpp BlockData.hpp ../include/minecraft-file.hpp)
list(APPEND mcfile_test_link_libraries "z")

set(CMAKE_REQUIRED_FLAGS "-lstdc++fs")
check_c_compiler_flag("" TEST_RESULT_STDCXXFS)
if (TEST_RESULT_STDCXXFS AND "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  list(APPEND mcfile_test_link_libraries stdc++fs)
endif()

target_link_libraries(mcfile-test ${mcfile_test_link_libraries})
