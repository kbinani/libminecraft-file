cmake_minimum_required(VERSION 3.0)
project(mcfile)

include_directories(../include ext/svpng ext/hwm.task ext/colormap-shaders/include)
add_definitions(-std=c++17)
add_executable(mcfile-example main.cpp ../include/minecraft-file.hpp)
target_link_libraries(mcfile-example z)
