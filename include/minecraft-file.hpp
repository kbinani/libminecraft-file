#pragma once

/// @file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <initializer_list>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#if __has_include(<leveldb/db.h>)
#include <leveldb/db.h>
#endif

#if __has_include(<zopfli.h>)
#include <zopfli.h>
#endif

// clang-format off

#include <mcfile/endianness.hpp>
#include <mcfile/algorithm.hpp>
#include <mcfile/file.hpp>
#include <mcfile/stream/input-stream.hpp>
#include <mcfile/stream/output-stream.hpp>
#include <mcfile/stream/file-input-stream.hpp>
#include <mcfile/stream/file-output-stream.hpp>
#include <mcfile/stream/byte-stream.hpp>
#include <mcfile/stream/input-stream-reader.hpp>
#include <mcfile/stream/output-stream-writer.hpp>
#include <mcfile/compression.hpp>
#include <mcfile/string.hpp>
#include <mcfile/nbt/tag-factory.hpp>
#include <mcfile/nbt/tag.hpp>
#include <mcfile/nbt/end-tag.hpp>
#include <mcfile/nbt/detail/scalar-tag.hpp>
#include <mcfile/nbt/byte-tag.hpp>
#include <mcfile/nbt/short-tag.hpp>
#include <mcfile/nbt/int-tag.hpp>
#include <mcfile/nbt/long-tag.hpp>
#include <mcfile/nbt/float-tag.hpp>
#include <mcfile/nbt/double-tag.hpp>
#include <mcfile/nbt/detail/vector-tag.hpp>
#include <mcfile/nbt/byte-array-tag.hpp>
#include <mcfile/nbt/int-array-tag.hpp>
#include <mcfile/nbt/long-array-tag.hpp>
#include <mcfile/nbt/string-tag.hpp>
#include <mcfile/nbt/list-tag.hpp>
#include <mcfile/nbt/compound-tag.hpp>
#include <mcfile/nbt/tag-factory-impl.hpp>
#include <mcfile/coordinate.hpp>
#include <mcfile/blocks/block-id.hpp>
#include <mcfile/blocks/minecraft.hpp>
#include <mcfile/blocks/from-name.hpp>
#include <mcfile/blocks/name.hpp>
#include <mcfile/je/block.hpp>
#include <mcfile/biomes/biome-id.hpp>
#include <mcfile/biomes/minecraft.hpp>
#include <mcfile/biomes/from-int.hpp>
#include <mcfile/biomes/name.hpp>
#include <mcfile/dimension.hpp>
#include <mcfile/je/chunk-section.hpp>
#include <mcfile/je/chunksection/chunk-section-112.hpp>
#include <mcfile/je/chunksection/chunk-section-113-base.hpp>
#include <mcfile/je/chunksection/block-states-parser-113.hpp>
#include <mcfile/je/chunksection/block-states-parser-116.hpp>
#include <mcfile/je/chunksection/chunk-section-113.hpp>
#include <mcfile/je/chunksection/chunk-section-116.hpp>
#include <mcfile/je/chunksection/chunk-section-generator.hpp>
#include <mcfile/je/set-block-options.hpp>
#include <mcfile/pos3.hpp>
#include <mcfile/je/ticking-block.hpp>
#include <mcfile/je/chunk.hpp>
#include <mcfile/je/writable-chunk.hpp>
#include <mcfile/je/mca-data-source.hpp>
#include <mcfile/je/region.hpp>
#include <mcfile/je/world.hpp>

#include <mcfile/be/db-key.hpp>
#include <mcfile/be/db-interface.hpp>
#include <mcfile/be/db.hpp>
#include <mcfile/be/block.hpp>
#include <mcfile/be/pending-tick.hpp>
#include <mcfile/be/sub-chunk.hpp>
#include <mcfile/be/biome.hpp>
#include <mcfile/be/chunk.hpp>

#include <mcfile/nbt/print-as-json.hpp>
#include <mcfile/pos2.hpp>
#include <mcfile/je/cached-chunk-loader.hpp>
