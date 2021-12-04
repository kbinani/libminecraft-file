#pragma once

namespace mcfile::be {

class DbKey {
public:
    enum class Tag : uint8_t {
        SubChunk = 0x2f,
        Version = 0x2c,
        VersionLegacy = 0x76,
        Data2DLegacy = 0x2d,
        Data2D = 0x2b,
        BiomeState = 0x35,
        ChecksumsLegacy = 0x3b,
        BlockEntity = 0x31,
        Entity = 0x32,
        PendingTicks = 0x33,
        FinalizedState = 0x36,
        StructureBounds = 0x39,
        Tag3d = 0x3d,
    };

    static std::string SubChunk(int32_t chunkX, int32_t chunkY, int32_t chunkZ, Dimension dim) {
        std::vector<char> b;
        PlaceXZTag(b, chunkX, chunkZ, Tag::SubChunk);
        SetDimension(b, dim);
        int8_t const y = Clamp<int8_t>(chunkY);
        uint8_t rawY = *(uint8_t *)&y;
        b.push_back(rawY);
        return std::string(b.data(), b.size());
    }

    static std::string Version(int32_t chunkX, int32_t chunkZ, Dimension dim) {
        return ComposeChunkKey(chunkX, chunkZ, dim, Tag::Version);
    }

    [[deprecated]] static std::string VersionLegacy(int32_t chunkX, int32_t chunkZ, Dimension dim) {
        return ComposeChunkKey(chunkX, chunkZ, dim, Tag::VersionLegacy);
    }

    static std::string Data2D(int32_t chunkX, int32_t chunkZ, Dimension dim) {
        return ComposeChunkKey(chunkX, chunkZ, dim, Tag::Data2D);
    }

    static std::string BiomeState(int32_t chunkX, int32_t chunkZ, Dimension dim) {
        return ComposeChunkKey(chunkX, chunkZ, dim, Tag::BiomeState);
    }

    [[deprecated]] static std::string ChecksumsLegacy(int32_t chunkX, int32_t chunkZ, Dimension dim) {
        return ComposeChunkKey(chunkX, chunkZ, dim, Tag::ChecksumsLegacy);
    }

    static std::string Portals() { return "portals"; }

    static std::string BlockEntity(int32_t chunkX, int32_t chunkZ, Dimension dim) {
        return ComposeChunkKey(chunkX, chunkZ, dim, Tag::BlockEntity);
    }

    static std::string Entity(int32_t chunkX, int32_t chunkZ, Dimension dim) {
        return ComposeChunkKey(chunkX, chunkZ, dim, Tag::Entity);
    }

    static std::string PendingTicks(int32_t chunkX, int32_t chunkZ, Dimension dim) {
        return ComposeChunkKey(chunkX, chunkZ, dim, Tag::PendingTicks);
    }

    static std::string Map(int64_t id) { return std::string("map_") + std::to_string(id); }

    static std::string TheEnd() { return "TheEnd"; }

    static std::string AutonomousEntities() { return "AutonomousEntities"; }

    static std::string FinalizedState(int32_t chunkX, int32_t chunkZ, Dimension dim) {
        return ComposeChunkKey(chunkX, chunkZ, dim, Tag::FinalizedState);
    }

    static std::string StructureBounds(int32_t chunkX, int32_t chunkZ, Dimension dim) {
        return ComposeChunkKey(chunkX, chunkZ, dim, Tag::StructureBounds);
    }

    static std::string LocalPlayer() { return "~local_player"; }

    static std::string MobEvents() { return "mobevents"; }

    static std::string ComposeChunkKey(int32_t chunkX, int32_t chunkZ, Dimension dim, uint8_t tag) {
        std::vector<char> b;
        PlaceXZTag(b, chunkX, chunkZ, tag);
        SetDimension(b, dim);
        return std::string(b.data(), b.size());
    }

    static std::string ComposeChunkKey(int32_t chunkX, int32_t chunkZ, Dimension dim, Tag tag) {
        return ComposeChunkKey(chunkX, chunkZ, dim, static_cast<uint8_t>(tag));
    }

    static std::optional<DbKey> Parse(std::string const &key) {
        switch (key.size()) {
        case 9: {
            if (key == "BiomeData" || key == "Overworld" || key == "mobevents") {
                DbKey k;
                k.fIsTagged = false;
                k.fUnTagged = key;
                return k;
            } else {
                uint8_t tag = key[8];
                int32_t cx = *(int32_t *)key.data();
                int32_t cz = *(int32_t *)(key.data() + 4);
                DbKey k;
                k.fIsTagged = true;
                k.fTagged.fTag = tag;
                k.fTagged.fDimension = 0;
                k.fTagged.fChunk.fX = cx;
                k.fTagged.fChunk.fX = cz;
                return k;
            }
        }
        case 10: {
            uint8_t tag = key[8];
            if (key == "scoreboard" || tag != 0x2f) {
                DbKey k;
                k.fIsTagged = false;
                k.fUnTagged = key;
                return k;
            } else {
                int32_t cx = *(int32_t *)key.data();
                int32_t cz = *(int32_t *)(key.data() + 4);
                uint8_t rawY = key[9];
                int8_t y = *(int8_t *)&rawY;
                DbKey k;
                k.fIsTagged = true;
                k.fTagged.fTag = tag;
                k.fTagged.fDimension = 0;
                k.fTagged.fSubChunk.fX = cx;
                k.fTagged.fSubChunk.fY = y;
                k.fTagged.fSubChunk.fZ = cz;
                return k;
            }
        }
        case 13: {
            if (key == "~local_player") {
                DbKey k;
                k.fIsTagged = false;
                k.fUnTagged = key;
                return k;
            } else {
                uint8_t tag = key[12];
                int32_t cx = *(int32_t *)key.data();
                int32_t cz = *(int32_t *)(key.data() + 4);
                int32_t dim = *(int32_t *)(key.data() + 8);
                DbKey k;
                k.fIsTagged = true;
                k.fTagged.fTag = tag;
                k.fTagged.fDimension = dim;
                k.fTagged.fChunk.fX = cx;
                k.fTagged.fChunk.fZ = cz;
                return k;
            }
        }
        case 14: {
            uint8_t tag = key[12];
            if (tag == 0x2f) {
                int32_t cx = *(int32_t *)key.data();
                int32_t cz = *(int32_t *)(key.data() + 4);
                int32_t dim = *(int32_t *)(key.data() + 8);
                uint8_t rawY = key[13];
                int8_t y = *(int8_t *)&rawY;
                DbKey k;
                k.fIsTagged = true;
                k.fTagged.fTag = tag;
                k.fTagged.fDimension = dim;
                k.fTagged.fSubChunk.fX = cx;
                k.fTagged.fSubChunk.fY = y;
                k.fTagged.fSubChunk.fZ = cz;
                return k;
            } else {
                DbKey k;
                k.fIsTagged = false;
                k.fUnTagged = key;
                return k;
            }
        }
        default:
            DbKey k;
            k.fIsTagged = false;
            k.fUnTagged = key;
            return k;
        }
    }

    std::string toString() const {
        using namespace std;
        if (!fIsTagged) {
            return fUnTagged;
        }
        ostringstream s;
        static unordered_map<uint8_t, string> const sMap = {
            {0x2f, "SubChunk"},
            {0x2b, "Data2D"},
            {0x2c, "ChunkVersion"},
            {0x2d, "Data2DLegacy"},
            {0x31, "BlockEntity"},
            {0x32, "Entity"},
            {0x33, "PendingTicks"},
            {0x35, "BiomeState"},
            {0x36, "FinalizedState"},
            {0x39, "StructureBounds"},
            {0x3a, "RandomTicks"},
            {0x3b, "ChecksumsLegacy"},
            {0x3d, "(unknown)"},
            {0x76, "ChunkVersionLegacy"},
        };
        auto found = sMap.find(fTagged.fTag);
        if (found == sMap.end()) {
            s << "(unknown)";
        } else {
            s << found->second;
        }
        s << "(" << (int)fTagged.fTag << "/0x" << hex << (int)fTagged.fTag << dec << ") [";
        if (fTagged.fTag == 0x2f) {
            s << fTagged.fSubChunk.fX << ", " << fTagged.fSubChunk.fY << ", " << fTagged.fSubChunk.fZ << "]";
        } else {
            s << fTagged.fChunk.fX << ", " << fTagged.fChunk.fZ << "]";
        }
        s << " ";
        switch (fTagged.fDimension) {
        case 0:
            s << "overworld";
            break;
        case 1:
            s << "end";
            break;
        case -1:
            s << "nether(legacy)";
            break;
        case 2:
            s << "nether";
            break;
        default:
            s << "(unknown: " << fTagged.fDimension << ")";
            break;
        }
        return s.str();
    }

private:
    DbKey() {
    }

    static void PlaceXZTag(std::vector<char> &out, int32_t chunkX, int32_t chunkZ, Tag tag) {
        PlaceXZTag(out, chunkX, chunkZ, static_cast<uint8_t>(tag));
    }

    static void PlaceXZTag(std::vector<char> &out, int32_t chunkX, int32_t chunkZ, uint8_t tag) {
        out.clear();
        out.resize(9);
        *(uint32_t *)out.data() = mcfile::Int32LEFromNative(*(uint32_t *)&chunkX);
        *(uint32_t *)(out.data() + 4) = mcfile::Int32LEFromNative(*(uint32_t *)&chunkZ);
        out[8] = tag;
    }

    static void SetDimension(std::vector<char> &out, Dimension dim) {
        if (dim == Dimension::Overworld) {
            return;
        }
        assert(out.size() == 9);
        uint8_t tag = out[8];
        out.resize(13);
        uint32_t v;
        switch (dim) {
        case Dimension::Nether:
            v = 2; // this was "1" for version < 1.18
            break;
        case Dimension::End:
        default:
            v = static_cast<uint8_t>(dim);
            break;
        }
        *(uint32_t *)(out.data() + 8) = mcfile::Int32LEFromNative(v);
        out[12] = tag;
    }

public:
    bool fIsTagged;
    struct {
        uint8_t fTag;
        int32_t fDimension;
        union {
            struct {
                int fX;
                int fZ;
            } fChunk;
            struct {
                int fX;
                int fY;
                int fZ;
            } fSubChunk;
        };

        std::optional<Dimension> dimension() const {
            switch (fDimension) {
            case 0:
                return Dimension::Overworld;
            case 1:
                return Dimension::End;
            case -1:
            case 2:
                return Dimension::Nether;
            default:
                return std::nullopt;
            }
        }
    } fTagged;
    std::string fUnTagged;
};
} // namespace mcfile::be
