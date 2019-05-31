cmake_minimum_required(VERSION 3.0)
project(mcfile)

include_directories(../include  ext/doctest)
add_definitions(-std=c++14)
add_executable(mcfile-test main.cpp ../include/minecraft-file.hpp)
target_link_libraries(mcfile-test z)
