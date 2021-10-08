#pragma once

namespace mcfile::je {

class World {
public:
    explicit World(std::filesystem::path const &rootDirectory)
        : fRootDirectory(rootDirectory) {
    }

    World(std::string const &) = delete;

    std::shared_ptr<Region> region(int regionX, int regionZ) const {
        namespace fs = std::filesystem;
        fs::path regionFilePath = fRootDirectory / "region" / ("r." + std::to_string(regionX) + "." + std::to_string(regionZ) + ".mca");
        return Region::MakeRegion(regionFilePath);
    }

    std::shared_ptr<Region> regionAtBlock(int blockX, int blockZ) const {
        int rx = Coordinate::RegionFromBlock(blockX);
        int rz = Coordinate::RegionFromBlock(blockZ);
        return region(rx, rz);
    }

    std::shared_ptr<Chunk> chunkAtBlock(int blockX, int blockZ) const {
        auto const &region = regionAtBlock(blockX, blockZ);
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
        auto const &r = region(rx, rz);
        if (!r) {
            return nullptr;
        }
        return r->chunkAt(chunkX, chunkZ);
    }

    std::shared_ptr<WritableChunk> writableChunkAtBlock(int blockX, int blockZ) const {
        auto const &region = regionAtBlock(blockX, blockZ);
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
        auto const &r = region(rx, rz);
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
                return region->loadAllChunks(error, [minX, minZ, maxX, maxZ, callback](Chunk const &chunk) {
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

    bool eachRegions(std::function<bool(int rx, int rz, std::filesystem::path file)> callback) const {
        namespace fs = std::filesystem;
        std::error_code ec;

        auto regionDir = fRootDirectory / "region";
        if (!fs::exists(regionDir, ec)) {
            return true;
        }
        if (ec) {
            return false;
        }
        ec.clear();

        fs::directory_iterator itr(regionDir, ec);
        if (ec) {
            return false;
        }
        for (auto const &sub : itr) {
            auto xz = Region::RegionXZFromFile(sub.path());
            if (!xz) {
                continue;
            }
            if (!callback(xz->fX, xz->fZ, sub.path())) {
                return false;
            }
        }
        return true;
    }

    bool eachRegions(std::function<bool(std::shared_ptr<Region> const &)> callback) const {
        return eachRegions([callback](int rx, int rz, std::filesystem::path path) {
            auto region = Region::MakeRegion(path, rx, rz);
            if (!region) {
                return true;
            }
            if (!callback(region)) {
                return false;
            }
            return true;
        });
    }

public:
    std::filesystem::path const fRootDirectory;
};

} // namespace mcfile::je
