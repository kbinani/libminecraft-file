#pragma once

namespace mcfile::je {

class McaEditor {
    std::vector<bool> fUsedSectors;
    std::vector<uint32_t> fIndex;
    struct Data {
        std::string fData;
        RegionCompression::CompressionType fCompressionType;
    };
    std::vector<Data> fCompressedChunks;

public:
    static std::unique_ptr<McaEditor> Open(std::filesystem::path const &path) {
        std::vector<bool> usedSectors;
        usedSectors.resize(1024, false);
        usedSectors[0] = true;
        usedSectors[1] = true;

        std::vector<uint32_t> index(1024, 0);
        std::vector<Data> compressedChunks(1024);

        ScopedFile file(File::Open(path, File::Mode::Read));
        if (!file) {
            return std::unique_ptr<McaEditor>(new McaEditor(usedSectors, index, compressedChunks));
        }
        if (fread(index.data(), 4096, 1, file.get()) != 1) {
            return nullptr;
        }
        for (int i = 0; i < index.size(); i++) {
            index[i] = U32FromBE(index[i]);
        }
        for (int i = 0; i < index.size(); i++) {
            uint32_t loc = index[i];
            uint32_t offset = SectorOffset(loc);
            uint32_t sectors = NumSectors(loc);
            if (usedSectors.size() < offset + sectors) {
                usedSectors.resize(offset + sectors, false);
            }
            for (int j = 0; j < sectors; j++) {
                assert(usedSectors[offset + j] == false);
                usedSectors[offset + j] = true;
            }
            if (sectors == 0) {
                continue;
            }
            if (offset < 2) {
                continue;
            }
            if (!File::Fseek(file.get(), offset * 4096, SEEK_SET)) {
                return nullptr;
            }
            uint32_t size;
            if (fread(&size, sizeof(size), 1, file.get()) != 1) {
                return nullptr;
            }
            size = U32FromBE(size);
            if (size < 2) {
                return nullptr;
            }
            uint8_t comp;
            if (fread(&comp, sizeof(comp), 1, file.get()) != 1) {
                return nullptr;
            }
            auto type = RegionCompression::CompressionTypeFromUint8(comp);
            if (!type) {
                return nullptr;
            }
            std::string tmp;
            tmp.resize(size - 1);
            if (fread(tmp.data(), tmp.size(), 1, file.get()) != 1) {
                return nullptr;
            }
            compressedChunks[i].fData.swap(tmp);
            compressedChunks[i].fCompressionType = *type;
        }
        return std::unique_ptr<McaEditor>(new McaEditor(usedSectors, index, compressedChunks));
    }

    std::shared_ptr<nbt::CompoundTag> get(int localX, int localZ) {
        if (localX < 0 || 32 <= localX || localZ < 0 || 32 <= localZ) {
            return nullptr;
        }
        int index = localZ * 32 + localX;
        Data const &data = fCompressedChunks[index];
        if (data.fData.empty()) {
            return nullptr;
        }
        return RegionCompression::Decompress(data.fCompressionType, data.fData, Encoding::Java);
    }

    std::shared_ptr<nbt::CompoundTag> extract(int localX, int localZ) {
        auto tag = get(localX, localZ);
        if (!tag) {
            return nullptr;
        }
        int index = localZ * 32 + localX;
        uint32_t loc = fIndex[index];
        uint32_t sectorOffset = SectorOffset(loc);
        uint32_t numSectors = NumSectors(loc);
        fIndex[index] = 0;
        std::string().swap(fCompressedChunks[index].fData);
        fCompressedChunks[index].fCompressionType = RegionCompression::CompressionType::None;
        if (fUsedSectors.size() < sectorOffset + numSectors) {
            fUsedSectors.resize(sectorOffset + numSectors);
        }
        for (int i = 0; i < numSectors; i++) {
            fUsedSectors[sectorOffset + i] = false;
        }
        return tag;
    }

    bool insert(int localX, int localZ, nbt::CompoundTag const &tag) {
        if (localX < 0 || 32 <= localX || localZ < 0 || 32 <= localZ) {
            return false;
        }
        int index = localZ * 32 + localX;

        auto data = nbt::CompoundTag::WriteDeflateCompressed(tag, Encoding::Java);
        if (!data) {
            return false;
        }
        if (data->empty()) {
            return false;
        }

        uint32_t numSectors = Ceil<uint32_t>(data->size() + 1 + 4, (uint32_t)4096) / 4096;
        int offset = -1;
        for (int i = 2; i + numSectors < fUsedSectors.size(); i++) {
            bool ok = true;
            for (int j = 0; j < numSectors; j++) {
                if (fUsedSectors[i + j]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                offset = i;
                break;
            }
        }
        if (offset < 0) {
            int lastTrue = fUsedSectors.size() - 1;
            for (int i = fUsedSectors.size() - 1; i >= 0; i--) {
                if (fUsedSectors[i]) {
                    lastTrue = i;
                    break;
                }
            }
            offset = lastTrue + 1;
            int add = (int)(offset + numSectors) - (int)fUsedSectors.size();
            for (int i = 0; i < add; i++) {
                fUsedSectors.push_back(false);
            }
        }
        fCompressedChunks[index].fData.swap(*data);
        fCompressedChunks[index].fCompressionType = RegionCompression::CompressionType::Deflate;
        for (int i = 0; i < numSectors; i++) {
            fUsedSectors[offset + i] = true;
        }
        uint32_t loc = (0xFFFFFF00 & (offset << 8)) | (0xFF & numSectors);
        fIndex[index] = loc;
        return true;
    }

    bool write(std::filesystem::path const &path) {
        ScopedFile file(File::Open(path, File::Mode::Write));
        if (!file) {
            return false;
        }
        for (uint32_t loc : fIndex) {
            loc = U32BEFromNative(loc);
            if (fwrite(&loc, sizeof(loc), 1, file.get()) != 1) {
                return false;
            }
        }
        for (int i = 0; i < fIndex.size(); i++) {
            uint32_t loc = fIndex[i];
            auto offset = SectorOffset(loc);
            auto numSectors = NumSectors(loc);
            if (offset < 2 || numSectors == 0) {
                continue;
            }

            Data const &data = fCompressedChunks[i];
            if (data.fData.empty()) {
                continue;
            }

            if (!File::Fseek(file.get(), offset * 4096, SEEK_SET)) {
                return false;
            }
            uint32_t size = U32BEFromNative(data.fData.size() + 1);
            if (fwrite(&size, sizeof(size), 1, file.get()) != 1) {
                return false;
            }
            uint8_t comp = static_cast<uint8_t>(data.fCompressionType);
            if (fwrite(&comp, sizeof(comp), 1, file.get()) != 1) {
                return false;
            }
            if (fwrite(data.fData.data(), data.fData.size(), 1, file.get()) != 1) {
                return false;
            }
        }
        int lastTrue = 2;
        for (int i = fUsedSectors.size() - 1; i >= 2; i--) {
            if (fUsedSectors[i]) {
                lastTrue = i;
                break;
            }
        }
        return File::Ftruncate(file.get(), (lastTrue + 1) * 4096);
    }

    static bool Load(std::filesystem::path const &path, int localX, int localZ, std::string &out, RegionCompression::CompressionType &type) {
        if (localX < 0 || 32 <= localX || localZ < 0 || 32 <= localZ) {
            return false;
        }
        stream::FileInputStream s(path);
        if (!s.valid()) {
            return false;
        }
        int index = localZ * 32 + localX;
        if (!s.seek(index * 4)) {
            return false;
        }
        uint32_t loc;
        if (!s.read(&loc, sizeof(loc))) {
            return false;
        }
        loc = U32FromBE(loc) >> 8;
        if (!s.seek(loc * 4096)) {
            return false;
        }
        uint32_t size;
        if (!s.read(&size, sizeof(size))) {
            return false;
        }
        size = U32FromBE(size);
        if (size < 2) {
            return false;
        }
        uint8_t comp;
        if (!s.read(&comp, sizeof(comp))) {
            return false;
        }
        auto t = RegionCompression::CompressionTypeFromUint8(comp);
        if (!t) {
            return false;
        }
        std::string tmp;
        tmp.resize(size - 1);
        if (!s.read(tmp.data(), tmp.size())) {
            return false;
        }
        tmp.swap(out);
        type = *t;
        return true;
    }

    static bool Load(std::filesystem::path const &path, int localX, int localZ, std::shared_ptr<nbt::CompoundTag> &out) {
        std::string tmp;
        RegionCompression::CompressionType type;
        if (!Load(path, localX, localZ, tmp, type)) {
            return false;
        }
        auto ret = RegionCompression::Decompress(type, tmp, Encoding::Java);
        if (!ret) {
            return false;
        }
        ret.swap(out);
        return true;
    }

private:
    McaEditor(std::vector<bool> &usedSectors, std::vector<uint32_t> &index, std::vector<Data> &compressedChunks) {
        fUsedSectors.swap(usedSectors);
        fIndex.swap(index);
        fCompressedChunks.swap(compressedChunks);
    }

    static uint32_t SectorOffset(uint32_t loc) {
        return 0xFFFFFF & (loc >> 8);
    }

    static uint32_t NumSectors(uint32_t loc) {
        return 0xFF & loc;
    }

    template<class T>
    static T Ceil(T v, T div) {
        T odd = v % div;
        if (odd == 0) {
            return v;
        } else {
            return v + (div - odd);
        }
    }
};

} // namespace mcfile::je
