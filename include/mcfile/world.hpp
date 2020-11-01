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

    bool eachRegions(std::function<void(std::shared_ptr<Region> const&)> callback) const {
        namespace fs = mcfile::detail::filesystem;
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
            callback(region);
        }
        return true;
    }

public:
    std::string const fRootDirectory;
};

} // namespace mcfile
