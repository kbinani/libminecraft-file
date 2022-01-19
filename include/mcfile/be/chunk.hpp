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

    std::optional<biomes::BiomeId> biomeAt(int x, int y, int z) const {
        if (!fBiomes) {
            return std::nullopt;
        }
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return std::nullopt;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        size_t i = offsetZ * 16 + offsetX;
        return fBiomes->get(offsetX, y, offsetZ);
    }

    static std::shared_ptr<Chunk> Load(int chunkX, int chunkZ, Dimension d, DbInterface &db) {
        return LoadImpl(chunkX, chunkZ, d, db);
    }

#if __has_include(<leveldb/db.h>)
    static std::shared_ptr<Chunk> Load(int chunkX, int chunkZ, Dimension d, leveldb::DB *db) {
        using namespace std;
        using namespace leveldb;

        if (!db) {
            return nullptr;
        }

        WrapDb wdb(db);
        return LoadImpl(chunkX, chunkZ, d, wdb);
    }

    static bool ForAll(leveldb::DB *db, Dimension dim, std::function<void(int cx, int cz)> cb) {
        return ForAllImpl(db, dim, [cb](int cx, int cz) { cb(cx, cz); return true; });
    }

    static bool ForAll(leveldb::DB *db, Dimension dim, std::function<bool(int cx, int cz)> cb) {
        return ForAllImpl(db, dim, cb);
    }
#endif

private:
    Chunk(int32_t chunkX, int32_t chunkZ,
          std::shared_ptr<SubChunk> subChunks[kNumSubChunks],
          std::unordered_map<Pos3i, std::shared_ptr<mcfile::nbt::CompoundTag>, Pos3iHasher> &blockEntities,
          std::vector<std::shared_ptr<mcfile::nbt::CompoundTag>> &entities,
          std::unordered_map<Pos3i, PendingTick, Pos3iHasher> &pendingTicks,
          std::shared_ptr<BiomeMap> &biomes)
        : fChunkX(chunkX)
        , fChunkZ(chunkZ) {
        for (int8_t i = 0; i < kNumSubChunks; i++) {
            fSubChunks[i] = subChunks[i];
        }
        fBlockEntities.swap(blockEntities);
        fEntities.swap(entities);
        fPendingTicks.swap(pendingTicks);
        fBiomes.swap(biomes);
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

    static std::shared_ptr<BiomeMap> LoadBiomes(int chunkX, int chunkZ, Dimension d, DbInterface &db) {
        using namespace std;
        using namespace mcfile::stream;
        auto data2D = db.get(DbKey::Data2D(chunkX, chunkZ, d));
        if (data2D) {
            return BiomeMap::Decode(-4, *data2D, 512);
        }

        auto data2DLegacy = db.get(DbKey::Data2DLegacy(chunkX, chunkZ, d));
        if (!data2DLegacy) {
            return nullptr;
        }
        auto ret = make_shared<BiomeMap>(0, 15);
        vector<uint8_t> buffer;
        copy(data2DLegacy->begin(), data2DLegacy->end(), back_inserter(buffer));
        auto stream = make_shared<ByteStream>(buffer);
        InputStreamReader sr(stream, {.fLittleEndian = true});
        for (int i = 0; i < 256; i++) {
            uint16_t v = 0;
            if (!sr.read(&v)) {
                return nullptr;
            }
        }
        for (int z = 0; z < 16; z++) {
            for (int x = 0; x < 16; x++) {
                uint8_t b = 0;
                if (!sr.read(&b)) {
                    return nullptr;
                }
                auto biome = Biome::FromUint32(b);
                for (int y = 0; y < 256; y++) {
                    ret->set(x, y, z, biome);
                }
            }
        }
        return ret;
    }

    static std::shared_ptr<Chunk> LoadImpl(int chunkX, int chunkZ, Dimension d, DbInterface &db) {
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

        auto biomes = LoadBiomes(chunkX, chunkZ, d, db);

        return shared_ptr<Chunk>(new Chunk(chunkX, chunkZ, subChunks, blockEntities, entities, pendingTicks, biomes));
    }

#if __has_include(<leveldb/db.h>)
    static bool ForAllImpl(leveldb::DB *db, Dimension dim, std::function<bool(int cx, int cz)> cb) {
        using namespace std;
        using namespace leveldb;

        if (!db) {
            return false;
        }
        ReadOptions ro;
        unique_ptr<Iterator> itr(db->NewIterator(ro));
        if (!itr) {
            return false;
        }
        int const expectedKeyLength = dim == Dimension::Overworld ? 9 : 13;
        for (itr->SeekToFirst(); itr->Valid(); itr->Next()) {
            Slice key = itr->key();
            if (key.size() != expectedKeyLength) {
                continue;
            }
            auto k = DbKey::Parse(key.ToString());
            if (!k) {
                continue;
            }
            if (!k->fIsTagged) {
                continue;
            }
            uint8_t const tag = k->fTagged.fTag;
            if (tag != static_cast<uint8_t>(DbKey::Tag::Version) && tag != static_cast<uint8_t>(DbKey::Tag::VersionLegacy)) {
                continue;
            }
            if (k->fTagged.fDimension != dim) {
                continue;
            }
            int cx = k->fTagged.fChunk.fX;
            int cz = k->fTagged.fChunk.fZ;
            if (!cb(cx, cz)) {
                return false;
            }
        }
        return true;
    }
#endif

public:
    int32_t const fChunkX;
    int32_t const fChunkZ;
    std::unordered_map<Pos3i, std::shared_ptr<mcfile::nbt::CompoundTag>, Pos3iHasher> fBlockEntities;
    std::vector<std::shared_ptr<mcfile::nbt::CompoundTag>> fEntities;
    std::unordered_map<Pos3i, PendingTick, Pos3iHasher> fPendingTicks;
    std::shared_ptr<BiomeMap> fBiomes;

private:
    std::shared_ptr<SubChunk> fSubChunks[kNumSubChunks];
};

} // namespace mcfile::be
