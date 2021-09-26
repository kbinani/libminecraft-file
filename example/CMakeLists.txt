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

list(APPEND zopflimca_link_libraries z libzopfli)
if (NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
  list(APPEND zopflimca_link_libraries pthread)
endif()
add_executable(zopflimca zopflimca.cpp ../include/minecraft-file.hpp)
target_link_libraries(zopflimca ${zopflimca_link_libraries})
