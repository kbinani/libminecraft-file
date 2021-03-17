#pragma once

namespace mcfile {
namespace detail {

class ChunkDataSource {
public:
    ChunkDataSource(int chunkX, int chunkZ, uint32_t timestamp, long offset, long length)
        : fChunkX(chunkX), fChunkZ(chunkZ), fTimestamp(timestamp), fOffset(offset), fLength(length) {
    }

    std::shared_ptr<Chunk> load(::mcfile::stream::InputStreamReader &reader) const {
        if (!reader.valid()) {
            return nullptr;
        }
        if (!reader.seek(fOffset + sizeof(uint32_t))) {
            return nullptr;
        }
        uint8_t compressionType;
        if (!reader.read(&compressionType)) {
            return nullptr;
        }
        if (compressionType != 2) {
            return nullptr;
        }
        std::vector<uint8_t> buffer(fLength - 1);
        if (!reader.read(buffer)) {
            return nullptr;
        }
        if (!Compression::decompress(buffer)) {
            return nullptr;
        }
        auto root = std::make_shared<nbt::CompoundTag>();
        auto bs = std::make_shared<stream::ByteStream>(buffer);
        auto sr = std::make_shared<stream::InputStreamReader>(bs);
        root->read(*sr);
        if (!root->valid()) {
            return nullptr;
        }
        return Chunk::MakeChunk(fChunkX, fChunkZ, root);
    }
    
    bool load(::mcfile::stream::InputStreamReader &reader, std::function<void(Chunk &chunk)> callback) const {
        if (!reader.valid()) {
            return false;
        }
        if (!reader.seek(fOffset + sizeof(uint32_t))) {
            return false;
        }
        uint8_t compressionType;
        if (!reader.read(&compressionType)) {
            return false;
        }
        if (compressionType != 2) {
            return false;
        }
        std::vector<uint8_t> buffer(fLength - 1);
        if (!reader.read(buffer)) {
            return false;
        }
        if (!Compression::decompress(buffer)) {
            return false;
        }
        auto root = std::make_shared<nbt::CompoundTag>();
        auto bs = std::make_shared<stream::ByteStream>(buffer);
        auto sr = std::make_shared<stream::InputStreamReader>(bs);
        root->read(*sr);
        if (!root->valid()) {
            return false;
        }
        auto chunk = Chunk::MakeChunk(fChunkX, fChunkZ, root);
        if (!chunk) {
            return false;
        }
        callback(*chunk);
        return true;
    }

public:
    int const fChunkX;
    int const fChunkZ;
    uint32_t const fTimestamp;
    long const fOffset;
    long const fLength;
};

} // namespace detail
} // namespace mcfile
