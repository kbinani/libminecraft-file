cmake_minimum_required(VERSION 3.0)
project(mcfile)

if (NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

add_subdirectory(ext/zopfli EXCLUDE_FROM_ALL)

include_directories(../include ext/thread-pool/include)

add_executable(mcfile-example main.cpp ../include/minecraft-file.hpp)
target_link_libraries(mcfile-example z)

add_executable(zopflimca zopflimca.cpp ../include/minecraft-file.hpp)
target_link_libraries(zopflimca z libzopfli)
