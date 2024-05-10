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
        return subChunk->blockAtUnchecked(offsetX, offsetY, offsetZ);
    }

    std::shared_ptr<Block const> blockAt(Pos3i pos) const {
        return blockAt(pos.fX, pos.fY, pos.fZ);
    }

    std::optional<biomes::BiomeId> biomeAt(int x, int y, int z) const {
        assert(fWhat.fBiomes);
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
        return fBiomes->getUnchecked(offsetX, y, offsetZ);
    }

    std::shared_ptr<mcfile::nbt::CompoundTag const> blockEntityAt(Pos3i pos) const {
        assert(fWhat.fBlockEntities);
        return blockEntityAt(pos.fX, pos.fY, pos.fZ);
    }

    std::shared_ptr<mcfile::nbt::CompoundTag const> blockEntityAt(int x, int y, int z) const {
        assert(fWhat.fBlockEntities);
        auto found = fBlockEntities.find(Pos3i(x, y, z));
        if (found == fBlockEntities.end()) {
            return nullptr;
        }
        return found->second;
    }

    struct LoadWhat {
        bool fBiomes = true;
        bool fPendingTicks = true;
        bool fBlockEntities = true;
        bool fEntities = true;

        static LoadWhat Everything() {
            LoadWhat ret;
            return ret;
        }
    };

    static std::shared_ptr<Chunk> Load(int chunkX, int chunkZ, Dimension d, DbInterface &db, Encoding encoding, LoadWhat what = LoadWhat::Everything()) {
        return LoadImpl(chunkX, chunkZ, d, db, encoding, what);
    }

#if __has_include(<leveldb/db.h>)
    static std::shared_ptr<Chunk> Load(int chunkX, int chunkZ, Dimension d, leveldb::DB *db, Encoding encoding, LoadWhat what = LoadWhat::Everything()) {
        using namespace std;
        using namespace leveldb;

        if (!db) {
            return nullptr;
        }

        WrapDb wdb(db);
        return LoadImpl(chunkX, chunkZ, d, wdb, encoding, what);
    }

    static bool ForAll(leveldb::DB *db, Dimension dim, std::function<bool(int cx, int cz)> cb) {
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
            if (!k.fIsTagged) {
                continue;
            }
            uint8_t const tag = k.fTagged.fTag;
            if (tag != static_cast<uint8_t>(DbKey::Tag::Version) && tag != static_cast<uint8_t>(DbKey::Tag::VersionLegacy)) {
                continue;
            }
            if (k.fTagged.fDimension != dim) {
                continue;
            }
            int cx = k.fTagged.fChunk.fX;
            int cz = k.fTagged.fChunk.fZ;
            if (!cb(cx, cz)) {
                return false;
            }
        }
        return true;
    }

    static bool Exists(leveldb::DB *db, int cx, int cz, Dimension dim) {
        if (!db) {
            return false;
        }
        std::string v;
        if (auto st = db->Get({}, DbKey::Version(cx, cz, dim), &v); st.ok()) {
            return true;
        }
        if (auto st = db->Get({}, DbKey::VersionLegacy(cx, cz, dim), &v); st.ok()) {
            return true;
        }
        return false;
    }
#endif

    int minBlockX() const { return fChunkX * 16; }
    int maxBlockX() const { return fChunkX * 16 + 15; }

    int minBlockY() const { return fChunkY * 16; }
    int maxBlockY() const { return fChunkY < 0 ? 319 : 255; }

    int minBlockZ() const { return fChunkZ * 16; }
    int maxBlockZ() const { return fChunkZ * 16 + 15; }

    int32_t currentTick() const {
        assert(fWhat.fPendingTicks);
        return fCurrentTick;
    }

    std::unordered_map<Pos3i, std::shared_ptr<mcfile::nbt::CompoundTag>, Pos3iHasher> &blockEntities() {
        assert(fWhat.fBlockEntities);
        return fBlockEntities;
    }

    std::unordered_map<Pos3i, std::shared_ptr<mcfile::nbt::CompoundTag>, Pos3iHasher> const &blockEntities() const {
        assert(fWhat.fBlockEntities);
        return fBlockEntities;
    }

    std::vector<std::shared_ptr<mcfile::nbt::CompoundTag>> &entities() {
        assert(fWhat.fEntities);
        return fEntities;
    }

    std::vector<std::shared_ptr<mcfile::nbt::CompoundTag>> const &entities() const {
        assert(fWhat.fEntities);
        return fEntities;
    }

    std::vector<PendingTick> &pendingTicks() {
        assert(fWhat.fPendingTicks);
        return fPendingTicks;
    }

    std::vector<PendingTick> const &pendingTicks() const {
        assert(fWhat.fPendingTicks);
        return fPendingTicks;
    }

private:
    Chunk(Dimension dim,
          int32_t chunkX, int32_t chunkY, int32_t chunkZ,
          std::vector<std::shared_ptr<SubChunk>> &subChunks,
          std::unordered_map<Pos3i, std::shared_ptr<mcfile::nbt::CompoundTag>, Pos3iHasher> &blockEntities,
          std::vector<std::shared_ptr<mcfile::nbt::CompoundTag>> &entities,
          std::vector<PendingTick> &pendingTicks,
          std::shared_ptr<BiomeMap> &biomes,
          uint8_t version,
          LoadWhat what)
        : fChunkX(chunkX)
        , fChunkZ(chunkZ)
        , fVersion(version)
        , fChunkY(chunkY)
        , fWhat(what) {
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

    static void LoadBlockEntities(int chunkX, int chunkZ, Dimension d, DbInterface &db, Encoding encoding, std::unordered_map<Pos3i, std::shared_ptr<nbt::CompoundTag>, Pos3iHasher> &result) {
        using namespace std;
        using namespace mcfile::nbt;
        using namespace mcfile::stream;

        auto value = db.get(DbKey::BlockEntity(chunkX, chunkZ, d));
        if (!value) {
            return;
        }
        auto stream = make_shared<ByteStream>(*value);
        InputStreamReader sr(stream, encoding);
        CompoundTag::ReadSequentialUntilEos(sr, [&result](shared_ptr<CompoundTag> const &tag) {
            auto bx = tag->int32(u8"x");
            auto by = tag->int32(u8"y");
            auto bz = tag->int32(u8"z");
            if (!bx || !by || !bz) {
                return;
            }
            Pos3i pos(*bx, *by, *bz);
            result.insert(make_pair(pos, tag));
        });
    }

    static void LoadEntities(int chunkX, int chunkZ, Dimension d, DbInterface &db, Encoding encoding, std::vector<std::shared_ptr<nbt::CompoundTag>> &result) {
        using namespace std;
        using namespace mcfile::nbt;
        using namespace mcfile::stream;

        if (auto entities = db.get(DbKey::Entity(chunkX, chunkZ, d)); entities) {
            CompoundTag::ReadSequentialUntilEos(*entities, encoding, [&result](shared_ptr<CompoundTag> const &tag) {
                result.push_back(tag);
            });
        }
        if (auto digp = db.get(DbKey::Digp(chunkX, chunkZ, d)); digp) {
            DbKey::EnumerateActorprefixKeys(*digp, [&db, &result, encoding](int index, string const &key, bool &stop) {
                auto actor = db.get(DbKey::Actorprefix(key));
                if (!actor) {
                    return;
                }
                auto c = CompoundTag::Read(*actor, encoding);
                if (!c) {
                    return;
                }
                result.push_back(c);
            });
        }
    }

    static void LoadPendingTicks(int chunkX, int chunkZ, Dimension d, DbInterface &db, Encoding encoding, std::vector<PendingTick> *outTickList, int32_t *outCurrentTick) {
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

        auto pendingTicks = CompoundTag::Read(*value, encoding);
        if (!pendingTicks) {
            return;
        }
        auto currentTick = pendingTicks->int32(u8"currentTick");
        auto tickList = pendingTicks->listTag(u8"tickList");
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

    static std::shared_ptr<BiomeMap> LoadBiomes(int chunkX, int chunkY, int chunkZ, Dimension d, DbInterface &db, Encoding encoding) {
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
        if (data2D->size() < 768) {
            return nullptr;
        }
        auto ret = make_shared<BiomeMap>(0, 15);
        string_view buffer(data2D->data() + 512, 256);
        mcfile::biomes::BiomeId biomesXZ[16][16];
        int idx = 0;
        for (int z = 0; z < 16; z++) {
            for (int x = 0; x < 16; x++, idx++) {
                auto id = Biome::FromUint32(buffer[idx]);
                biomesXZ[x][z] = id;
            }
        }
        ret->set2D(biomesXZ);
        return ret;
    }

    static std::shared_ptr<Chunk> LoadImpl(int chunkX, int chunkZ, Dimension d, DbInterface &db, Encoding encoding, LoadWhat what) {
        using namespace std;
        using namespace mcfile::stream;
        using namespace mcfile::nbt;

        vector<shared_ptr<SubChunk>> subChunks;

        int8_t possibleMinY = d == Dimension::Overworld ? -4 : 0;
        int8_t possibleMaxY = d == Dimension::Overworld ? 19 : 15;

        int8_t actualMinY = 0;

        uint8_t chunkVersion = 0;
        if (auto version = db.get(DbKey::Version(chunkX, chunkZ, d)); version) {
            if (version->size() == 1) {
                chunkVersion = Mem::Read<uint8_t>(*version, 0);
            } else {
                return nullptr;
            }
        } else if (auto legacyVersion = db.get(DbKey::VersionLegacy(chunkX, chunkZ, d)); legacyVersion) {
            if (legacyVersion->size() == 1) {
                chunkVersion = Mem::Read<uint8_t>(*legacyVersion, 0);
            } else {
                return nullptr;
            }
        }

        // chunkVersion:
        // 15: Minecraft-1.13.1.5.Appx
        // 22: Minecraft-1.17.0.2.Appx

        for (int8_t y = possibleMinY; y <= possibleMaxY; y++) {
            auto subChunkData = db.get(DbKey::SubChunk(chunkX, y, chunkZ, d));
            if (!subChunkData) {
                continue;
            }
            auto subChunk = SubChunk::Parse(*subChunkData, y, encoding);
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
        if (what.fBlockEntities) {
            LoadBlockEntities(chunkX, chunkZ, d, db, encoding, blockEntities);
        }

        vector<shared_ptr<CompoundTag>> entities;
        if (what.fEntities) {
            LoadEntities(chunkX, chunkZ, d, db, encoding, entities);
        }

        vector<PendingTick> pendingTicks;
        int32_t currentTick = 0;
        if (what.fPendingTicks) {
            LoadPendingTicks(chunkX, chunkZ, d, db, encoding, &pendingTicks, &currentTick);
        }

        shared_ptr<BiomeMap> biomes;
        if (what.fBiomes) {
            biomes = LoadBiomes(chunkX, chunkY, chunkZ, d, db, encoding);
        }

        auto ret = shared_ptr<Chunk>(new Chunk(d, chunkX, chunkY, chunkZ, subChunks, blockEntities, entities, pendingTicks, biomes, chunkVersion, what));
        ret->fCurrentTick = currentTick;
        return ret;
    }

public:
    int32_t const fChunkX;
    int32_t const fChunkZ;
    uint8_t const fVersion;
    std::vector<std::shared_ptr<SubChunk>> fSubChunks;
    int32_t fChunkY;
    LoadWhat const fWhat;

private:
    std::unordered_map<Pos3i, std::shared_ptr<mcfile::nbt::CompoundTag>, Pos3iHasher> fBlockEntities;
    std::vector<std::shared_ptr<mcfile::nbt::CompoundTag>> fEntities;
    std::vector<PendingTick> fPendingTicks;
    std::shared_ptr<BiomeMap> fBiomes;
    int32_t fCurrentTick = 0;
};

} // namespace mcfile::be
