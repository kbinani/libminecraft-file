#pragma once

namespace mcfile {

class World {
public:
    explicit World(std::string const& rootDirectory)
        : fRootDirectory(rootDirectory)
    {
    }

    std::shared_ptr<Region> region(int regionX, int regionZ) const {
        std::ostringstream ss;
        ss << fRootDirectory << "/region/r." << regionX << "." << regionZ << ".mca";
        auto fileName = ss.str();
        return Region::MakeRegion(fileName);
    }

    std::shared_ptr<Region> regionAtBlock(int blockX, int blockZ) const {
        int rx = Coordinate::RegionFromBlock(blockX);
        int rz = Coordinate::RegionFromBlock(blockZ);
        return region(rx, rz);
    }

    std::shared_ptr<Chunk> chunkAtBlock(int blockX, int blockZ) const {
        auto const& region = regionAtBlock(blockX, blockZ);
        if (!region) {
            return nullptr;
        }
        int cx = Coordinate::ChunkFromBlock(blockX);
        int cz = Coordinate::ChunkFromBlock(blockZ);
        return region->chunkAt(cx, cz);
    }

    std::shared_ptr<Chunk> chunkAt(int chunkX, int chunkZ) const {
        int rx = Coordinate::RegionFromChunk(chunkX);
        int rz = Coordinate::RegionFromChunk(chunkZ);
        auto const& r = region(rx, rz);
        if (!r) {
            return nullptr;
        }
        return r->chunkAt(chunkX, chunkZ);
    }

    std::shared_ptr<WritableChunk> writableChunkAtBlock(int blockX, int blockZ) const {
        auto const& region = regionAtBlock(blockX, blockZ);
        if (!region) {
            return nullptr;
        }
        int cx = Coordinate::ChunkFromBlock(blockX);
        int cz = Coordinate::ChunkFromBlock(blockZ);
        return region->writableChunkAt(cx, cz);
    }

    std::shared_ptr<WritableChunk> writableChunkAt(int chunkX, int chunkZ) const {
        int rx = Coordinate::RegionFromChunk(chunkX);
        int rz = Coordinate::RegionFromChunk(chunkZ);
        auto const& r = region(rx, rz);
        if (!r) {
            return nullptr;
        }
        return r->writableChunkAt(chunkX, chunkZ);
    }

    bool eachBlock(int minX, int minZ, int maxX, int maxZ, std::function<bool(int x, int y, int z, std::shared_ptr<Block const>)> callback) const {
        if (minX > maxX || minZ > maxZ) {
            return false;
        }
        int const minRegionX = Coordinate::RegionFromBlock(minX);
        int const maxRegionX = Coordinate::RegionFromBlock(maxX);
        int const minRegionZ = Coordinate::RegionFromBlock(minZ);
        int const maxRegionZ = Coordinate::RegionFromBlock(maxZ);
        for (int regionZ = minRegionZ; regionZ <= maxRegionZ; regionZ++) {
            for (int regionX = minRegionX; regionX <= maxRegionX; regionX++) {
                auto region = this->region(regionX, regionZ);
                if (!region) {
                    continue;
                }
                bool error = false;
                return region->loadAllChunks(error, [minX, minZ, maxX, maxZ, callback](Chunk const& chunk) {
                    for (int y = 0; y < 256; y++) {
                        for (int z = std::max(minZ, chunk.minBlockZ()); z <= std::min(maxZ, chunk.maxBlockZ()); z++) {
                            for (int x = std::max(minX, chunk.minBlockX()); x <= std::min(maxX, chunk.maxBlockX()); x++) {
                                auto block = chunk.blockAt(x, y, z);
                                if (!callback(x, y, z, block)) {
                                    return false;
                                }
                            }
                        }
                    }
                    return true;
                });
            }
        }
        return true;
    }

    bool eachRegions(std::function<bool(std::shared_ptr<Region> const&)> callback) const {
        namespace fs = std::filesystem;
        auto regionDir = fs::path(fRootDirectory).append("region");
        if (!fs::exists(regionDir)) {
            return  true;
        }
        fs::directory_iterator it(regionDir);
        for (auto const& path : it) {
            auto region = Region::MakeRegion(path.path().string());
            if (!region) {
                continue;
            }
            if (!callback(region)) {
                return false;
            }
        }
        return true;
    }

public:
    std::string const fRootDirectory;
};

} // namespace mcfile
