#pragma once

/// @file

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <string.h>
#include <cstdint>
#include <cassert>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include <limits>
#include <functional>
#include <cmath>
#include <optional>
#include <initializer_list>
#include <filesystem>
#include <type_traits>
#include <unordered_map>

#include <mcfile/detail/endianness.hpp>
#include <mcfile/stream/input-stream.hpp>
#include <mcfile/stream/output-stream.hpp>
#include <mcfile/stream/file-input-stream.hpp>
#include <mcfile/stream/file-output-stream.hpp>
#include <mcfile/stream/byte-stream.hpp>
#include <mcfile/stream/input-stream-reader.hpp>
#include <mcfile/stream/output-stream-writer.hpp>
#include <mcfile/detail/compression.hpp>
#include <mcfile/detail/string.hpp>
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
#include <mcfile/block.hpp>
#include <mcfile/biomes/biome-id.hpp>
#include <mcfile/biomes/minecraft.hpp>
#include <mcfile/biomes/from-int.hpp>
#include <mcfile/biomes/name.hpp>
#include <mcfile/chunk-section.hpp>
#include <mcfile/chunksection/chunk-section-112.hpp>
#include <mcfile/chunksection/chunk-section-113-base.hpp>
#include <mcfile/chunksection/block-states-parser-113.hpp>
#include <mcfile/chunksection/block-states-parser-116.hpp>
#include <mcfile/chunksection/chunk-section-113.hpp>
#include <mcfile/chunksection/chunk-section-116.hpp>
#include <mcfile/chunksection/chunk-section-generator.hpp>
#include <mcfile/set-block-options.hpp>
#include <mcfile/pos3.hpp>
#include <mcfile/chunk.hpp>
#include <mcfile/detail/mca-data-source.hpp>
#include <mcfile/region.hpp>
#include <mcfile/world.hpp>

#include <mcfile/nbt/print-as-json.hpp>
#include <mcfile/pos2.hpp>
#include <mcfile/cached-chunk-loader.hpp>
