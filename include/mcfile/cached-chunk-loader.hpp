#pragma once

namespace mcfile {

class CachedChunkLoader {
public:
    explicit CachedChunkLoader(Region region) : fRegion(region) {
    }

    void addToCache(std::shared_ptr<Chunk const> const& chunk) {
        Pos2i pos(chunk->fChunkX, chunk->fChunkZ);
        fCache.insert(std::make_pair(pos, chunk));
    }

    std::shared_ptr<Block const> blockAt(int bx, int by, int bz) {
        auto chunk = chunkAtBlock(bx, bz);
        if (chunk) {
            return chunk->blockAt(bx, by, bz);
        } else {
            return nullptr;
        }
    }

    void eachTileEntitiesInRange(Pos3i blockCenter, int radius, std::function<bool(std::shared_ptr<nbt::CompoundTag> const&, Pos3i)> callback) {
        if (radius < 0) {
            return;
        }
        int minBx = blockCenter.fX - radius;
        int maxBx = blockCenter.fX + radius;
        int minCx = Coordinate::ChunkFromBlock(minBx);
        int maxCx = Coordinate::ChunkFromBlock(maxBx);

        int minBy = blockCenter.fY - radius;
        int maxBy = blockCenter.fY + radius;

        int minBz = blockCenter.fZ - radius;
        int maxBz = blockCenter.fZ + radius;
        int minCz = Coordinate::ChunkFromBlock(minBz);
        int maxCz = Coordinate::ChunkFromBlock(maxBz);

        for (int cx = minCx; cx <= maxCx; cx++) {
            for (int cz = minCz; cz <= maxCz; cz++) {
                auto const& chunk = chunkAt(cx, cz);
                if (!chunk) {
                    continue;
                }
                for (auto const& it : chunk->fTileEntities) {
                    auto pos = it.first;
                    if (pos.fX < minBx || maxBx < pos.fX) {
                        continue;
                    }
                    if (pos.fY < minBy || maxBy < pos.fY) {
                        continue;
                    }
                    if (pos.fZ < minBz || maxBz < pos.fZ) {
                        continue;
                    }
                    if (!callback(it.second, pos)) {
                        return;
                    }
                }
            }
        }
    }

private:
    std::shared_ptr<Chunk const> chunkAtBlock(int bx, int bz) {
        return chunkAt(Coordinate::ChunkFromBlock(bx), Coordinate::ChunkFromBlock(bz));
    }

    std::shared_ptr<Chunk const> chunkAt(int cx, int cz) {
        Pos2i pos(cx, cz);
        auto found = fCache.find(pos);
        if (found == fCache.end()) {
            auto chunk = fRegion.chunkAt(pos.fX, pos.fZ);
            fCache.insert(std::make_pair(pos, chunk));
            return chunk;
        } else {
            return found->second;
        }
    }

private:
    Region fRegion;
    std::unordered_map<Pos2i, std::shared_ptr<Chunk const>, Pos2iHasher> fCache;
};
}
