#pragma once

namespace mcfile::be {

class Chunk {
public:
    std::shared_ptr<Block const> blockAt(int x, int y, int z) const {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return nullptr;
        }
        auto subChunk = subChunkAtBlock(y);
        if (!subChunk) {
            return nullptr;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        int const offsetY = y - Coordinate::ChunkFromBlock(y) * 16;
        return subChunk->blockAt(offsetX, offsetY, offsetZ);
    }

    std::shared_ptr<Block const> blockAt(Pos3i pos) const {
        return blockAt(pos.fX, pos.fY, pos.fZ);
    }

    static std::shared_ptr<Chunk> Load(int chunkX, int chunkZ, Dimension d, DbInterface& db) {
        using namespace std;
        using namespace mcfile::stream;
        using namespace mcfile::nbt;

        shared_ptr<SubChunk> subChunks[16];
        fill_n(&subChunks[0], 16, nullptr);

        for (int y = 0; y < 16; y++) {
            auto subChunkData = db.get(DbKey::SubChunk(chunkX, y, chunkZ, d));
            if (!subChunkData) {
                continue;
            }
            auto subChunk = SubChunk::Parse(*subChunkData);
            if (!subChunk) {
                continue;
            }
            if (y < 16) {
                subChunks[y] = subChunk;
            }
        }

        unordered_map<Pos3i, shared_ptr<CompoundTag>, Pos3iHasher> blockEntities;
        LoadBlockEntities(chunkX, chunkZ, d, db, blockEntities);

        vector<shared_ptr<CompoundTag>> entities;
        LoadEntities(chunkX, chunkZ, d, db, entities);

        unordered_map<Pos3i, PendingTick, Pos3iHasher> pendingTicks;
        LoadPendingTicks(chunkX, chunkZ, d, db, pendingTicks);

        return shared_ptr<Chunk>(new Chunk(chunkX, chunkZ, subChunks, blockEntities, entities, pendingTicks));
    }

private:
    Chunk(int32_t chunkX, int32_t chunkZ,
          std::shared_ptr<SubChunk> subChunks[16],
          std::unordered_map<Pos3i, std::shared_ptr<mcfile::nbt::CompoundTag>, Pos3iHasher> &blockEntities,
          std::vector<std::shared_ptr<mcfile::nbt::CompoundTag>> &entities,
          std::unordered_map<Pos3i, PendingTick, Pos3iHasher> &pendingTicks)
        : fChunkX(chunkX)
        , fChunkZ(chunkZ)
    {
        for (int i = 0; i < 16; i++) {
            fSubChunks[i] = subChunks[i];
        }
        fBlockEntities.swap(blockEntities);
        fEntities.swap(entities);
        fPendingTicks.swap(pendingTicks);
    }

    std::shared_ptr<SubChunk> subChunkAtBlock(int y) const {
        int index = y / 16;
        if (0 <= index && index < 16) {
            return fSubChunks[index];
        } else {
            return nullptr;
        }
    }

    static void LoadBlockEntities(int chunkX, int chunkZ, Dimension d, DbInterface& db, std::unordered_map<Pos3i, std::shared_ptr<nbt::CompoundTag>, Pos3iHasher>& result) {
        using namespace std;
        using namespace mcfile::nbt;
        using namespace mcfile::stream;

        auto value = db.get(DbKey::BlockEntity(chunkX, chunkZ, d));
        if (!value) {
            return;
        }
        vector<uint8_t> buffer;
        copy(value->begin(), value->end(), back_inserter(buffer));
        auto stream = make_shared<ByteStream>(buffer);
        InputStreamReader sr(stream, {.fLittleEndian = true});
        CompoundTag::ReadUntilEos(sr, [&result](shared_ptr<CompoundTag> const& tag) {
            auto bx = tag->int32("x");
            auto by = tag->int32("y");
            auto bz = tag->int32("z");
            if (!bx || !by || !bz) {
                return;
            }
            Pos3i pos(*bx, *by, *bz);
            result.insert(make_pair(pos, tag));
        });
    }

    static void LoadEntities(int chunkX, int chunkZ, Dimension d, DbInterface& db, std::vector<std::shared_ptr<nbt::CompoundTag>> &result) {
        using namespace std;
        using namespace mcfile::nbt;
        using namespace mcfile::stream;

        auto value = db.get(DbKey::Entity(chunkX, chunkZ, d));
        if (!value) {
            return;
        }
        vector<uint8_t> buffer;
        copy(value->begin(), value->end(), back_inserter(buffer));
        auto stream = make_shared<ByteStream>(buffer);
        InputStreamReader sr(stream, {.fLittleEndian = true});
        CompoundTag::ReadUntilEos(sr, [&result](shared_ptr<CompoundTag> const& tag) {
            result.push_back(tag);
        });
    }

    static void LoadPendingTicks(int chunkX, int chunkZ, Dimension d, DbInterface&db, std::unordered_map<Pos3i, PendingTick, Pos3iHasher> &result) {
        using namespace std;
        using namespace mcfile::nbt;
        using namespace mcfile::stream;

        auto value = db.get(DbKey::PendingTicks(chunkX, chunkZ, d));
        if (!value) {
            return;
        }
        vector<uint8_t> buffer;
        copy(value->begin(), value->end(), back_inserter(buffer));
        auto stream = make_shared<ByteStream>(buffer);
        InputStreamReader sr(stream, {.fLittleEndian = true});
        CompoundTag::ReadUntilEos(sr, [&result](shared_ptr<CompoundTag> const& tag) {
            auto pair = PendingTick::FromCompound(*tag);
            if (!pair) {
                return;
            }
            result.insert(*pair);
        });
    }

public:
    int32_t const fChunkX;
    int32_t const fChunkZ;
    std::unordered_map<Pos3i, std::shared_ptr<mcfile::nbt::CompoundTag>, Pos3iHasher> fBlockEntities;
    std::vector<std::shared_ptr<mcfile::nbt::CompoundTag>> fEntities;
    std::unordered_map<Pos3i, PendingTick, Pos3iHasher> fPendingTicks;

private:
    std::shared_ptr<SubChunk> fSubChunks[16];
};

}
