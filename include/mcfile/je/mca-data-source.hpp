#pragma once

namespace mcfile::je {

class McaDataSource {
public:
    McaDataSource(int chunkX, int chunkZ, uint32_t timestamp, uint64_t offset, uint64_t length)
        : fChunkX(chunkX)
        , fChunkZ(chunkZ)
        , fTimestamp(timestamp)
        , fOffset(offset)
        , fLength(length) {
    }

    std::shared_ptr<nbt::CompoundTag> load(::mcfile::stream::InputStreamReader &reader) const {
        if (fLength == 0) {
            return nullptr;
        }
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
        return nbt::CompoundTag::ReadCompressed(buffer, reader.fEndian);
    }

    std::shared_ptr<Chunk> loadChunk(::mcfile::stream::InputStreamReader &reader) const {
        auto root = load(reader);
        if (root) {
            return Chunk::MakeChunk(fChunkX, fChunkZ, root);
        } else {
            return nullptr;
        }
    }

    bool loadChunk(::mcfile::stream::InputStreamReader &reader, std::function<void(Chunk &chunk)> callback) const {
        auto chunk = loadChunk(reader);
        if (!chunk) {
            return false;
        }
        callback(*chunk);
        return true;
    }

    std::shared_ptr<WritableChunk> loadWritableChunk(::mcfile::stream::InputStreamReader &reader) const {
        auto root = load(reader);
        if (root) {
            return WritableChunk::MakeChunk(fChunkX, fChunkZ, root);
        } else {
            return nullptr;
        }
    }

    bool loadWritableChunk(::mcfile::stream::InputStreamReader &reader, std::function<void(WritableChunk &chunk)> callback) const {
        auto chunk = loadWritableChunk(reader);
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
    uint64_t const fOffset;
    uint64_t const fLength;
};

} // namespace mcfile::je
