#pragma once

namespace mcfile::je {

class McaEditor {
    std::vector<bool> fUsedSectors;
    FILE *fStream = nullptr;
    std::vector<uint32_t> fIndex;

public:
    explicit McaEditor(std::filesystem::path const &path) {
        using namespace std;
        namespace fs = std::filesystem;
        fIndex.resize(1024, 0);
        fUsedSectors.push_back(true);
        fUsedSectors.push_back(true);

        if (fs::is_regular_file(path)) {
            fStream = File::Open(path, File::Mode::ReadWrite);
        } else {
            fStream = File::Open(path, File::Mode::Write);
        }
        if (!fStream) {
            return;
        }
        if (fread(fIndex.data(), 4096, 1, fStream) == 1) {
            for (int i = 0; i < fIndex.size(); i++) {
                fIndex[i] = U32FromBE(fIndex[i]);
            }
            for (uint32_t loc : fIndex) {
                uint32_t pos = SectorOffset(loc);
                uint32_t sectors = NumSectors(loc);
                if (fUsedSectors.size() < pos + sectors) {
                    fUsedSectors.resize(pos + sectors, false);
                }
                for (int j = 0; j < sectors; j++) {
                    assert(fUsedSectors[pos + j] == false);
                    fUsedSectors[pos + j] = true;
                }
            }
        }
    }

    ~McaEditor() {
        flush();
        if (fStream) {
            fclose(fStream);
        }
    }

    std::shared_ptr<nbt::CompoundTag> extract(int localX, int localZ) {
        if (localX < 0 || 32 <= localX || localZ < 0 || 32 <= localZ) {
            return nullptr;
        }
        if (!fStream) {
            return nullptr;
        }
        int index = localZ * 32 + localX;
        uint32_t loc = fIndex[index];
        uint32_t sectorOffset = SectorOffset(loc);
        uint32_t numSectors = NumSectors(loc);
        if (numSectors == 0) {
            return nullptr;
        }
        if (sectorOffset < 2) {
            return nullptr;
        }
        if (!File::Fseek(fStream, sectorOffset * 4096, SEEK_SET)) {
            return nullptr;
        }
        uint32_t size;
        if (fread(&size, sizeof(size), 1, fStream) != 1) {
            return nullptr;
        }
        size = U32FromBE(size);
        if (size < 2) {
            return nullptr;
        }
        uint8_t comp;
        if (fread(&comp, sizeof(comp), 1, fStream) != 1) {
            return nullptr;
        }
        if (comp != 2) {
            return nullptr;
        }
        std::string tmp;
        tmp.resize(size - 1);
        if (fread(tmp.data(), tmp.size(), 1, fStream) != 1) {
            return nullptr;
        }
        auto ret = nbt::CompoundTag::ReadCompressed(tmp, Endian::Big);
        if (!ret) {
            return nullptr;
        }
        fIndex[index] = 0;
        if (fUsedSectors.size() < sectorOffset + numSectors) {
            fUsedSectors.resize(sectorOffset + numSectors);
        }
        for (int i = 0; i < numSectors; i++) {
            fUsedSectors[sectorOffset + i] = false;
        }
        return ret;
    }

    bool insert(int localX, int localZ, nbt::CompoundTag const &tag) {
        if (localX < 0 || 32 <= localX || localZ < 0 || 32 <= localZ) {
            return false;
        }
        if (!fStream) {
            return false;
        }
        auto data = nbt::CompoundTag::WriteCompressed(tag, Endian::Big);
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
        if (!File::Fseek(fStream, offset * 4096, SEEK_SET)) {
            return false;
        }
        uint32_t size = U32BEFromNative(data->size() + 1);
        if (fwrite(&size, sizeof(size), 1, fStream) != 1) {
            return false;
        }
        uint8_t comp = 2;
        if (fwrite(&comp, sizeof(comp), 1, fStream) != 1) {
            return false;
        }
        if (fwrite(data->data(), data->size(), 1, fStream) != 1) {
            return false;
        }
        for (int i = 0; i < numSectors; i++) {
            fUsedSectors[offset + i] = true;
        }
        uint32_t loc = (0xFFFFFF00 & (offset << 8)) | (0xFF & numSectors);
        int index = localZ * 32 + localX;
        fIndex[index] = loc;
        return true;
    }

    bool flush() {
        if (!fStream) {
            return false;
        }
        if (!File::Fseek(fStream, 0, SEEK_SET)) {
            return false;
        }
        for (uint32_t loc : fIndex) {
            loc = U32BEFromNative(loc);
            if (fwrite(&loc, sizeof(loc), 1, fStream) != 1) {
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
        return File::Ftruncate(fStream, (lastTrue + 1) * 4096);
    }

    static bool Load(std::filesystem::path const &path, int localX, int localZ, std::string &out) {
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
        if (comp != 2) {
            return false;
        }
        std::string tmp;
        tmp.resize(size - 1);
        if (!s.read(tmp.data(), tmp.size())) {
            return false;
        }
        tmp.swap(out);
        return true;
    }

    static bool Load(std::filesystem::path const &path, int localX, int localZ, std::shared_ptr<nbt::CompoundTag> &out) {
        std::string tmp;
        if (!Load(path, localX, localZ, tmp)) {
            return false;
        }
        auto ret = nbt::CompoundTag::ReadCompressed(tmp, Endian::Big);
        if (!ret) {
            return false;
        }
        ret.swap(out);
        return true;
    }

private:
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
