#pragma once

namespace mcfile {

class RegionCompression {
    RegionCompression() = delete;

public:
    enum class CompressionType : uint8_t {
        Deflate = 2,
        None = 3,
        Lz4 = 4,
    };

    static std::optional<CompressionType> CompressionTypeFromUint8(uint8_t raw) {
        switch (raw) {
        case 2:
            return CompressionType::Deflate;
        case 3:
            return CompressionType::None;
        case 4:
            return CompressionType::Lz4;
        }
        return std::nullopt;
    }

    template<class T>
    static std::shared_ptr<nbt::CompoundTag> Decompress(CompressionType type, T const &data, Endian endian) {
        switch (type) {
        case CompressionType::Deflate:
            return nbt::CompoundTag::ReadDeflateCompressed(data, endian);
        case CompressionType::None:
            return nbt::CompoundTag::Read(data, endian);
        case CompressionType::Lz4:
            return nbt::CompoundTag::ReadLz4Compressed(data, endian);
        }
        return nullptr;
    }

    template<class T>
    static std::shared_ptr<nbt::CompoundTag> Decompress(uint8_t type, T const &data, Endian endian) {
        switch (type) {
        case 2:
            return nbt::CompoundTag::ReadDeflateCompressed(data, endian);
        case 3:
            return nbt::CompoundTag::Read(data, endian);
        case 4:
            return nbt::CompoundTag::ReadLz4Compressed(data, endian);
        }
        return nullptr;
    }
};
} // namespace mcfile
