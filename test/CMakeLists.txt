cmake_minimum_required(VERSION 3.0)
project(mcfile)

include(CheckCCompilerFlag)

include_directories(. ../include  ext/doctest)
add_definitions(-std=c++17)
add_executable(mcfile-test
  main.cpp
  Path.hpp
  Point3D.hpp
  BlockData.hpp
  # git ls-files | grep 'hpp$' | grep -v ^test/ | sed 's:\(.*\):  ../\1:g' | pbcopy
  ../include/mcfile/biomes/biome-id.hpp
  ../include/mcfile/biomes/from-int.hpp
  ../include/mcfile/biomes/minecraft.hpp
  ../include/mcfile/biomes/name.hpp
  ../include/mcfile/block.hpp
  ../include/mcfile/blocks/block-id.hpp
  ../include/mcfile/blocks/from-name.hpp
  ../include/mcfile/blocks/minecraft.hpp
  ../include/mcfile/blocks/name.hpp
  ../include/mcfile/cached-chunk-loader.hpp
  ../include/mcfile/chunk-section.hpp
  ../include/mcfile/chunk.hpp
  ../include/mcfile/chunksection/block-states-parser-113.hpp
  ../include/mcfile/chunksection/block-states-parser-116.hpp
  ../include/mcfile/chunksection/chunk-section-112.hpp
  ../include/mcfile/chunksection/chunk-section-113-base.hpp
  ../include/mcfile/chunksection/chunk-section-113.hpp
  ../include/mcfile/chunksection/chunk-section-116.hpp
  ../include/mcfile/chunksection/chunk-section-generator.hpp
  ../include/mcfile/coordinate.hpp
  ../include/mcfile/detail/compression.hpp
  ../include/mcfile/detail/endianness.hpp
  ../include/mcfile/detail/mca-data-source.hpp
  ../include/mcfile/detail/string.hpp
  ../include/mcfile/nbt/byte-array-tag.hpp
  ../include/mcfile/nbt/byte-tag.hpp
  ../include/mcfile/nbt/compound-tag.hpp
  ../include/mcfile/nbt/detail/scalar-tag.hpp
  ../include/mcfile/nbt/detail/vector-tag.hpp
  ../include/mcfile/nbt/double-tag.hpp
  ../include/mcfile/nbt/end-tag.hpp
  ../include/mcfile/nbt/float-tag.hpp
  ../include/mcfile/nbt/int-array-tag.hpp
  ../include/mcfile/nbt/int-tag.hpp
  ../include/mcfile/nbt/list-tag.hpp
  ../include/mcfile/nbt/long-array-tag.hpp
  ../include/mcfile/nbt/long-tag.hpp
  ../include/mcfile/nbt/print-as-json.hpp
  ../include/mcfile/nbt/short-tag.hpp
  ../include/mcfile/nbt/string-tag.hpp
  ../include/mcfile/nbt/tag-factory-impl.hpp
  ../include/mcfile/nbt/tag-factory.hpp
  ../include/mcfile/nbt/tag.hpp
  ../include/mcfile/pos2.hpp
  ../include/mcfile/pos3.hpp
  ../include/mcfile/region.hpp
  ../include/mcfile/set-block-options.hpp
  ../include/mcfile/stream/byte-stream.hpp
  ../include/mcfile/stream/file-input-stream.hpp
  ../include/mcfile/stream/file-output-stream.hpp
  ../include/mcfile/stream/input-stream-reader.hpp
  ../include/mcfile/stream/input-stream.hpp
  ../include/mcfile/stream/output-stream-writer.hpp
  ../include/mcfile/stream/output-stream.hpp
  ../include/mcfile/ticking-block.hpp
  ../include/mcfile/world.hpp
  ../include/minecraft-file.hpp)
list(APPEND mcfile_test_link_libraries "z")

set(CMAKE_REQUIRED_FLAGS "-lstdc++fs")
check_c_compiler_flag("" TEST_RESULT_STDCXXFS)
if (TEST_RESULT_STDCXXFS AND "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  list(APPEND mcfile_test_link_libraries stdc++fs)
endif()

target_link_libraries(mcfile-test ${mcfile_test_link_libraries})
