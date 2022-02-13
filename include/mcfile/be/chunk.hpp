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
        return fBiomes->get(offsetX, y, offsetZ);
    }

    std::shared_ptr<mcfile::nbt::CompoundTag const> blockEntityAt(Pos3i pos) const {
        return blockEntityAt(pos.fX, pos.fY, pos.fZ);
    }

    std::shared_ptr<mcfile::nbt::CompoundTag const> blockEntityAt(int x, int y, int z) const {
        auto found = fBlockEntities.find(Pos3i(x, y, z));
        if (found == fBlockEntities.end()) {
            return nullptr;
        }
        return found->second;
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

    int minBlockX() const { return fChunkX * 16; }
    int maxBlockX() const { return fChunkX * 16 + 15; }

    int minBlockY() const { return fChunkY * 16; }
    int maxBlockY() const { return fChunkY < 0 ? 319 : 255; }

    int minBlockZ() const { return fChunkZ * 16; }
    int maxBlockZ() const { return fChunkZ * 16 + 15; }

private:
    Chunk(Dimension dim,
          int32_t chunkX, int32_t chunkY, int32_t chunkZ,
          std::vector<std::shared_ptr<SubChunk>> &subChunks,
          std::unordered_map<Pos3i, std::shared_ptr<mcfile::nbt::CompoundTag>, Pos3iHasher> &blockEntities,
          std::vector<std::shared_ptr<mcfile::nbt::CompoundTag>> &entities,
          std::vector<PendingTick> &pendingTicks,
          std::shared_ptr<BiomeMap> &biomes)
        : fChunkX(chunkX)
        , fChunkY(chunkY)
        , fChunkZ(chunkZ) {
        int maxChunkY = fChunkY;
        for (auto const &subChunk : subChunks) {
            int cy = subChunk->fChunkY;
            maxChunkY = (std::max)(maxChunkY, cy);
        }
        int numChunks = maxChunkY - fChunkY + 1;
        fSubChunks.resize(numChunks);
        for (auto const &subChunk : subChunks) {
            int32_t cy = subChunk->fChunkY;
            int index = cy - fChunkY;
            if (0 <= index && index < fSubChunks.size()) {
                fSubChunks[index] = subChunk;
            }
        }
        fBlockEntities.swap(blockEntities);
        fEntities.swap(entities);
        fPendingTicks.swap(pendingTicks);
        fBiomes.swap(biomes);
    }

    std::shared_ptr<SubChunk> subChunkAtBlock(int y) const {
        int cy = Coordinate::ChunkFromBlock(y);
        int index = cy - fChunkY;
        if (0 <= index && index < fSubChunks.size()) {
            return fSubChunks[index];
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

    static void LoadPendingTicks(int chunkX, int chunkZ, Dimension d, DbInterface &db, std::vector<PendingTick> *outTickList, int32_t *outCurrentTick) {
        using namespace std;
        using namespace mcfile::nbt;
        using namespace mcfile::stream;

        if (!outTickList || !outCurrentTick) {
            return;
        }

        auto value = db.get(DbKey::PendingTicks(chunkX, chunkZ, d));
        if (!value) {
            return;
        }

        auto pendingTicks = CompoundTag::Read(*value, {.fLittleEndian = true});
        if (!pendingTicks) {
            return;
        }
        auto currentTick = pendingTicks->int32("currentTick");
        auto tickList = pendingTicks->listTag("tickList");
        if (!currentTick || !tickList) {
            return;
        }
        *outCurrentTick = *currentTick;

        outTickList->clear();
        for (auto const &it : *tickList) {
            auto tick = it->asCompound();
            if (!tick) {
                continue;
            }
            auto pendingTick = PendingTick::FromCompound(*tick);
            if (!pendingTick) {
                continue;
            }
            outTickList->push_back(*pendingTick);
        }
    }

    static std::shared_ptr<BiomeMap> LoadBiomes(int chunkX, int chunkY, int chunkZ, Dimension d, DbInterface &db) {
        using namespace std;
        using namespace mcfile::stream;
        auto data3D = db.get(DbKey::Data3D(chunkX, chunkZ, d));
        if (data3D) {
            return BiomeMap::Decode(chunkY, *data3D, 512);
        }

        auto data2D = db.get(DbKey::Data2D(chunkX, chunkZ, d));
        if (!data2D) {
            return nullptr;
        }
        auto ret = make_shared<BiomeMap>(0, 15);
        vector<uint8_t> buffer;
        copy(data2D->begin(), data2D->end(), back_inserter(buffer));
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

        vector<shared_ptr<SubChunk>> subChunks;

        int8_t possibleMinY = d == Dimension::Overworld ? -4 : 0;
        int8_t possibleMaxY = d == Dimension::Overworld ? 19 : 15;

        int8_t actualMinY = 0;

        for (int8_t y = possibleMinY; y <= possibleMaxY; y++) {
            auto subChunkData = db.get(DbKey::SubChunk(chunkX, y, chunkZ, d));
            if (!subChunkData) {
                continue;
            }
            auto subChunk = SubChunk::Parse(*subChunkData, y);
            if (!subChunk) {
                continue;
            }
            subChunks.push_back(subChunk);
            actualMinY = (std::min)(actualMinY, y);
        }

        int chunkY = 0;
        if (d == Dimension::Overworld && actualMinY < 0) {
            chunkY = -4;
        }

        unordered_map<Pos3i, shared_ptr<CompoundTag>, Pos3iHasher> blockEntities;
        LoadBlockEntities(chunkX, chunkZ, d, db, blockEntities);

        vector<shared_ptr<CompoundTag>> entities;
        LoadEntities(chunkX, chunkZ, d, db, entities);

        vector<PendingTick> pendingTicks;
        int32_t currentTick = 0;
        LoadPendingTicks(chunkX, chunkZ, d, db, &pendingTicks, &currentTick);

        auto biomes = LoadBiomes(chunkX, chunkY, chunkZ, d, db);

        auto ret = shared_ptr<Chunk>(new Chunk(d, chunkX, chunkY, chunkZ, subChunks, blockEntities, entities, pendingTicks, biomes));
        ret->fCurrentTick = currentTick;
        return ret;
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

    int32_t fCurrentTick = 0;
    std::vector<PendingTick> fPendingTicks;

    std::shared_ptr<BiomeMap> fBiomes;
    std::vector<std::shared_ptr<SubChunk>> fSubChunks;
    int32_t fChunkY;
};

} // namespace mcfile::be
