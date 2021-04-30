# libminecraft-file

![CI](https://github.com/kbinani/libminecraft-file/workflows/CI/badge.svg)

Header-only Minecraft region file reader / writer.

# example

```c++
#include <minecraft-file.hpp>
#include <iostream>

int main() {
    using namespace mcfile;

    // accessing game data of the nether dimension
    World nether("somewhere/.minecraft/saves/world/DIM-1");
    // read region file from world/DIM-1/regions/r.0.0.mca
    std::shared_ptr<Region> region = nether.region(0, 0);

    // query whether the chunk (1, 2) is stored in the mca file
    std::cout << region->chunkExists(1, 2) << std::endl; // ture/false

    std::shared_ptr<Chunk> invalid = region->chunkAt(999, 999); // nullptr because chunk [999, 999] is not stored in r.0.0.mca
    std::shared_ptr<Chunk> chunk = region->chunkAt(0, 0);

    std::shared_ptr<Block const> invalidBlock = chunk->blockAt(-1, -1, -1); // nullptr because of invalid coordinates

    // get block information at (x, y, z) = (0, 1, 2)
    std::shared_ptr<Block const> b00 = chunk->blockAt(0, 1, 2);
    std::cout << b00->fName << std::endl; // "minecraft:bedrock" etc.

    // get biome type at (x, y, z) = (1, 0, 2)
    biomes::BiomeId biome = chunk->biomeAt(1, 2);
    std::cout << biomes::Name(biome) << std::endl; // "minecraft:crimson_forest" etc.

    // accessing all chunks in the region
    bool err = false;
    region->loadAllChunks(err, [](Chunk const& c) -> bool {
        blocks::BlockId b = c.blockIdAt(c.minBlockX(), 0, c.maxBlockZ());
        std::cout << blocks::Name(b) << std::endl;
        return true; // continue
    });
    std::cout << err << std::endl; // false if succeeded

    // erase chunk data from region
    // DANGEROUS: remove chunk (0, 0) from the mca file and overwrite it
    // region->clearChunk(0, 0);

    // coordinate utilities
    std::cout << Coordinate::RegionFromBlock(-1) << std::endl; // -1

    // split the region file to compressed chunk files
    region->exportAllToCompressedNbt("./chunk_data_output_directory");
    // ./chunk_data_output_directory/c.0.0.nbt.z
    //   ├── c.0.0.nbt.z
    //   ├── c.0.1.nbt.z
    //   ...
    // *.nbt.z can be decompressed: `pigz -z -d < c.0.0.nbt.z > c.0.0.nbt`

    // restore *.mca file from compressed chunk files
    int const regionX = 0;
    int const regionZ = 1;
    Region::ConcatCompressedNbt(regionX, regionZ,
                                "./chunk_data_output_directory",
                                "./region_data_output_directory");
}
```

# dependency

- zlib

# license

- MIT
