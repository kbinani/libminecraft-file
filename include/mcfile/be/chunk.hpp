#pragma once

namespace mcfile::be {

class Chunk {
private:
    enum : int8_t {
        kMinChunkY = -4,
        kMaxChunkY = 19,

        kNumSubChunks = kMaxChunkY - kMinChunkY + 1,
    };

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

    biomes::BiomeId biomeAt(int x, int z) const {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return biomes::unknown;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        size_t i = offsetZ * 16 + offsetX;
        return fBiomeMap[i];
    }

    static std::shared_ptr<Chunk> Load(int chunkX, int chunkZ, Dimension d, DbInterface &db) {
        using namespace std;
        using namespace mcfile::stream;
        using namespace mcfile::nbt;

        shared_ptr<SubChunk> subChunks[kNumSubChunks];
        fill_n(&subChunks[0], kNumSubChunks, nullptr);

        for (int8_t y = kMinChunkY; y <= kMaxChunkY; y++) {
            auto subChunkData = db.get(DbKey::SubChunk(chunkX, y, chunkZ, d));
            if (!subChunkData) {
                continue;
            }
            auto subChunk = SubChunk::Parse(*subChunkData, y);
            if (!subChunk) {
                continue;
            }
            subChunks[y - kMinChunkY] = subChunk;
        }

        unordered_map<Pos3i, shared_ptr<CompoundTag>, Pos3iHasher> blockEntities;
        LoadBlockEntities(chunkX, chunkZ, d, db, blockEntities);

        vector<shared_ptr<CompoundTag>> entities;
        LoadEntities(chunkX, chunkZ, d, db, entities);

        unordered_map<Pos3i, PendingTick, Pos3iHasher> pendingTicks;
        LoadPendingTicks(chunkX, chunkZ, d, db, pendingTicks);

        vector<uint16_t> heightMap(256, (uint16_t)0);
        vector<biomes::BiomeId> biomeMap(256, biomes::minecraft::plains);
        auto data2D = db.get(DbKey::Data2D(chunkX, chunkZ, d));
        if (data2D) {
            vector<uint8_t> buffer;
            copy(data2D->begin(), data2D->end(), back_inserter(buffer));
            auto stream = make_shared<ByteStream>(buffer);
            InputStreamReader sr(stream, {.fLittleEndian = true});
            for (int i = 0; i < heightMap.size(); i++) {
                uint16_t v = 0;
                if (!sr.read(&v)) {
                    break;
                }
                heightMap[i] = v;
            }
            for (int i = 0; i < biomeMap.size(); i++) {
                uint8_t b = 0;
                if (!sr.read(&b)) {
                    break;
                }
                auto biome = Biome::FromUint8(b);
                biomeMap[i] = biome;
            }
        }

        return shared_ptr<Chunk>(new Chunk(chunkX, chunkZ, subChunks, blockEntities, entities, pendingTicks, heightMap, biomeMap));
    }

private:
    Chunk(int32_t chunkX, int32_t chunkZ,
          std::shared_ptr<SubChunk> subChunks[kNumSubChunks],
          std::unordered_map<Pos3i, std::shared_ptr<mcfile::nbt::CompoundTag>, Pos3iHasher> &blockEntities,
          std::vector<std::shared_ptr<mcfile::nbt::CompoundTag>> &entities,
          std::unordered_map<Pos3i, PendingTick, Pos3iHasher> &pendingTicks,
          std::vector<uint16_t> &heightMap,
          std::vector<biomes::BiomeId> &biomeMap)
        : fChunkX(chunkX)
        , fChunkZ(chunkZ) {
        for (int8_t i = 0; i < kNumSubChunks; i++) {
            fSubChunks[i] = subChunks[i];
        }
        fBlockEntities.swap(blockEntities);
        fEntities.swap(entities);
        fPendingTicks.swap(pendingTicks);
        fHeightMap.swap(heightMap);
        fBiomeMap.swap(biomeMap);
    }

    std::shared_ptr<SubChunk> subChunkAtBlock(int y) const {
        int index = Coordinate::ChunkFromBlock(y);
        if (kMinChunkY <= index && index <= kMaxChunkY) {
            return fSubChunks[index - kMinChunkY];
        } else {
            return nullptr;
        }
    }

    static void LoadBlockEntities(int chunkX, int chunkZ, Dimension d, DbInterface &db, std::unordered_map<Pos3i, std::shared_ptr<nbt::CompoundTag>, Pos3iHasher> &result) {
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
        CompoundTag::ReadUntilEos(sr, [&result](shared_ptr<CompoundTag> const &tag) {
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

    static void LoadEntities(int chunkX, int chunkZ, Dimension d, DbInterface &db, std::vector<std::shared_ptr<nbt::CompoundTag>> &result) {
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
        CompoundTag::ReadUntilEos(sr, [&result](shared_ptr<CompoundTag> const &tag) {
            result.push_back(tag);
        });
    }

    static void LoadPendingTicks(int chunkX, int chunkZ, Dimension d, DbInterface &db, std::unordered_map<Pos3i, PendingTick, Pos3iHasher> &result) {
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
        CompoundTag::ReadUntilEos(sr, [&result](shared_ptr<CompoundTag> const &tag) {
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
    std::vector<uint16_t> fHeightMap;
    std::vector<biomes::BiomeId> fBiomeMap;

private:
    std::shared_ptr<SubChunk> fSubChunks[kNumSubChunks];
};

} // namespace mcfile::be
