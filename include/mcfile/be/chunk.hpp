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

        auto checksums = db.get(DbKey::Checksums(chunkX, chunkZ, d));
        if (!checksums) {
            return nullptr;
        }

        shared_ptr<SubChunk> subChunks[16];
        fill_n(&subChunks[0], 16, nullptr);

        unordered_map<Pos3i, shared_ptr<CompoundTag>, Pos3iHasher> blockEntities;

        for (int i = 4; i + 10 < checksums->size(); i += 10) {
            uint8_t tag = *(uint8_t*)checksums->data();
            uint8_t y = *(uint8_t*)(checksums->data() + 1);
            uint64_t hash = *(uint64_t*)(checksums->data() + 2);
            if (tag == static_cast<uint8_t>(DbKey::Tag::SubChunk)) {
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
            } else if (tag == static_cast<uint8_t>(DbKey::Tag::BlockEntity)) {
                auto value = db.get(DbKey::BlockEntity(chunkX, chunkZ, d));
                if (!value) {
                    continue;
                }
                vector<uint8_t> buffer;
                copy(value->begin(), value->end(), back_inserter(buffer));
                auto stream = make_shared<ByteStream>(buffer);
                InputStreamReader sr(stream, {.fLittleEndian = true});
                while (true) {
                    uint8_t type;
                    if (!sr.read(&type)) {
                        break;
                    }
                    string name;
                    if (!sr.read(name)) {
                        break;
                    }
                    auto tag = make_shared<CompoundTag>();
                    tag->read(sr);
                    if (!tag->valid()) {
                        break;
                    }

                    auto bx = tag->int32("x");
                    auto by = tag->int32("y");
                    auto bz = tag->int32("z");
                    if (!bx || !by || !bz) {
                        continue;
                    }
                    Pos3i pos(*bx, *by, *bz);
                    blockEntities.insert(make_pair(pos, tag));
                }
            }
        }
        return shared_ptr<Chunk>(new Chunk(chunkX, chunkZ, subChunks, blockEntities));
    }

private:
    Chunk(int32_t chunkX, int32_t chunkZ,
          std::shared_ptr<SubChunk> subChunks[16],
          std::unordered_map<Pos3i, std::shared_ptr<mcfile::nbt::CompoundTag>, Pos3iHasher> &blockEntities)
        : fChunkX(chunkX)
        , fChunkZ(chunkZ)
    {
        for (int i = 0; i < 16; i++) {
            fSubChunks[i] = subChunks[i];
        }
        fBlockEntities.swap(blockEntities);
    }

    std::shared_ptr<SubChunk> subChunkAtBlock(int y) const {
        int index = y / 16;
        if (0 <= index && index < 16) {
            return fSubChunks[index];
        } else {
            return nullptr;
        }
    }

public:
    int32_t const fChunkX;
    int32_t const fChunkZ;
    std::unordered_map<Pos3i, std::shared_ptr<mcfile::nbt::CompoundTag>, Pos3iHasher> fBlockEntities;

private:
    std::shared_ptr<SubChunk> fSubChunks[16];
};

}
