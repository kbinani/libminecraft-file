#pragma once

namespace mcfile::je {

class CachedChunkLoader {
public:
    explicit CachedChunkLoader(McaEditor &editor)
        : fEditor(editor) {
    }

    void addToCache(std::shared_ptr<Chunk const> const &chunk) {
        Pos2i pos(chunk->fChunkX, chunk->fChunkZ);
        fCache.insert(std::make_pair(pos, chunk));
    }

    std::shared_ptr<Block const> blockAt(Pos3i pos) {
        auto chunk = chunkAtBlock(pos.fX, pos.fZ);
        if (chunk) {
            return chunk->blockAt(pos);
        } else {
            return nullptr;
        }
    }

    std::shared_ptr<nbt::CompoundTag> tileEntityAt(Pos3i pos) {
        auto chunk = chunkAtBlock(pos.fX, pos.fZ);
        if (!chunk) {
            return nullptr;
        }
        auto found = chunk->fTileEntities.find(pos);
        if (found == chunk->fTileEntities.end()) {
            return nullptr;
        }
        return found->second;
    }

private:
    std::shared_ptr<Chunk const> chunkAtBlock(int bx, int bz) {
        return chunkAt(Coordinate::ChunkFromBlock(bx), Coordinate::ChunkFromBlock(bz));
    }

    std::shared_ptr<Chunk const> chunkAt(int cx, int cz) {
        Pos2i pos(cx, cz);
        auto found = fCache.find(pos);
        if (found == fCache.end()) {
            int rx = Coordinate::RegionFromChunk(cx);
            int rz = Coordinate::RegionFromChunk(cz);
            int x = cx - rx * 32;
            int z = cz - rz * 32;
            auto nbt = fEditor.get(x, z);
            auto chunk = Chunk::MakeChunk(cx, cz, nbt);
            fCache.insert(std::make_pair(pos, chunk));
            return chunk;
        } else {
            return found->second;
        }
    }

private:
    McaEditor &fEditor;
    std::unordered_map<Pos2i, std::shared_ptr<Chunk const>, Pos2iHasher> fCache;
};
} // namespace mcfile::je
