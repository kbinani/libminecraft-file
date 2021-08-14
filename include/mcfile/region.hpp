#pragma once

namespace mcfile {

class Region {
public:
    Region(Region const& other) : fX(other.fX), fZ(other.fZ), fFilePath(other.fFilePath) {}

    Region& operator = (Region const& rh) = delete;

    using LoadChunkCallback = std::function<bool(Chunk &)>;

    bool loadAllChunks(bool& error, LoadChunkCallback callback) const {
        auto fs = std::make_shared<stream::FileInputStream>(fFilePath);
        stream::InputStreamReader sr(fs);
        for (int z = 0; z < 32; z++) {
            for (int x = 0; x < 32; x++) {
                if (!loadChunkImpl(x, z, sr, error, callback)) {
                    return false;
                }
            }
        }
        return true;
    }

    std::shared_ptr<Chunk> chunkAt(int chunkX, int chunkZ) const {
        int const localChunkX = chunkX - fX * 32;
        int const localChunkZ = chunkZ - fZ * 32;
        if (localChunkX < 0 || 32 <= localChunkX || localChunkZ < 0 || 32 <= localChunkZ) {
            return nullptr;
        }
        auto fs = std::make_shared<stream::FileInputStream>(fFilePath);
        stream::InputStreamReader sr(fs);
        std::shared_ptr<detail::McaDataSource> const& src = dataSource(localChunkX, localChunkZ, sr);
        if (!src) {
            return nullptr;
        }
        return src->loadChunk(sr);
    }

    bool entitiesAt(int chunkX, int chunkZ, std::vector<std::shared_ptr<nbt::CompoundTag>> &buffer) const {
        namespace fs = std::filesystem;
        int const localChunkX = chunkX - fX * 32;
        int const localChunkZ = chunkZ - fZ * 32;
        if (localChunkX < 0 || 32 <= localChunkX || localChunkZ < 0 || 32 <= localChunkZ) {
            return false;
        }
        std::string name = fs::path(fFilePath).filename().string();
        auto path = fs::path(fFilePath).remove_filename().parent_path().parent_path().append("entities").append(name);
        auto fin = std::make_shared<stream::FileInputStream>(path.string());
        stream::InputStreamReader sr(fin);
        std::shared_ptr<detail::McaDataSource> const& src = dataSource(localChunkX, localChunkZ, sr);
        if (!src) {
            return false;
        }
        auto compound = src->load(sr);
        if (!compound) {
            return false;
        }
        auto root = compound->compoundTag("");
        if (!root) {
            return false;
        }
        auto entities = root->listTag("Entities");
        if (!entities) {
            return false;
        }
        buffer.clear();
        for (auto const& e : entities->fValue) {
            if (e->id() != nbt::Tag::TAG_Compound) {
                continue;
            }
            auto c = std::dynamic_pointer_cast<nbt::CompoundTag>(e);
            if (!c) {
                continue;
            }
            buffer.push_back(c);
        }
        return true;
    }

    static std::shared_ptr<Region> MakeRegion(std::string const& filePath, int x, int z) {
        return std::shared_ptr<Region>(new Region(filePath, x, z));
    }

    static std::shared_ptr<Region> MakeRegion(std::string const& filePath) {
        // ../directory/r.5.13.mca
        namespace fs = std::filesystem;
        using mcfile::detail::String;
        using namespace std;

        string basename = fs::path(filePath).filename().string();

        vector<string> tokens = String::Split(basename, '.');
        if (tokens.size() != 4) {
            return nullptr;
        }
        if (tokens[0] != "r" || tokens[3] != "mca") {
            return nullptr;
        }

        int x, z;
        try {
            x = stoi(tokens[1]);
            z = stoi(tokens[2]);
        } catch (...) {
            return nullptr;
        }

        return shared_ptr<Region>(new Region(filePath, x, z));
    }

    int minBlockX() const { return fX * 32 * 16; }
    int maxBlockX() const { return (fX + 1) * 32 * 16 - 1; }
    int minBlockZ() const { return fZ * 32 * 16; }
    int maxBlockZ() const { return (fZ + 1) * 32 * 16 - 1; }

    int minChunkX() const { return fX * 32; }
    int maxChunkX() const { return (fX + 1) * 32 - 1; }
    int minChunkZ() const { return fZ * 32; }
    int maxChunkZ() const { return (fZ + 1) * 32 - 1; }

    bool clearChunk(int chunkX, int chunkZ) {
        int const localChunkX = chunkX - fX * 32;
        int const localChunkZ = chunkZ - fZ * 32;
        if (localChunkX < 0 || 32 <= localChunkX) {
            return false;
        }
        if (localChunkZ < 0 || 32 <= localChunkZ) {
            return false;
        }

        FILE *fp = fopen(fFilePath.c_str(), "r+b");
        if (!fp) {
            return false;
        }
        int const index = (localChunkX & 31) + (localChunkZ & 31) * 32;
        if (fseek(fp, 4 * index, SEEK_SET) != 0) {
            fclose(fp);
            return false;
        }
        uint32_t loc;
        if (fread(&loc, sizeof(loc), 1, fp) != 1) {
            fclose(fp);
            return false;
        }
        loc = detail::Int32FromBE(loc);
        if (loc == 0) {
            fclose(fp);
            return true;
        }
        long const sectorOffset = loc >> 8;
        if (fseek(fp, 4 * index, SEEK_SET) != 0) {
            fclose(fp);
            return false;
        }
        loc = 0;
        if (fwrite(&loc, sizeof(loc), 1, fp) != 1) {
            fclose(fp);
            return false;
        }

        if (fseek(fp, kSectorSize + 4 * index, SEEK_SET) != 0) {
            fclose(fp);
            return false;
        }
        uint32_t timestamp = 0;
        if (fwrite(&timestamp, sizeof(timestamp), 1, fp) != 1) {
            fclose(fp);
            return false;
        }
        if (fseek(fp, sectorOffset * kSectorSize, SEEK_SET) != 0) {
            fclose(fp);
            return false;
        }
        uint32_t chunkSize;
        if (fread(&chunkSize, sizeof(chunkSize), 1, fp) != 1) {
            fclose(fp);
            return false;
        }
        chunkSize = detail::Int32FromBE(chunkSize);
        for (uint32_t i = 0; i < chunkSize; i++) {
            uint8_t zero = 0;
            if (fwrite(&zero, sizeof(zero), 1, fp) != 1) {
                fclose(fp);
                return false;
            }
        }

        fclose(fp);
        return true;
    }
    
    bool chunkExists(int chunkX, int chunkZ) const {
        int const localChunkX = chunkX - fX * 32;
        int const localChunkZ = chunkZ - fZ * 32;
        if (localChunkX < 0 || 32 <= localChunkX) {
            return false;
        }
        if (localChunkZ < 0 || 32 <= localChunkZ) {
            return false;
        }
        auto fs = std::make_shared<stream::FileInputStream>(fFilePath);
        stream::InputStreamReader sr(fs);
        auto data = dataSource(localChunkX, localChunkZ, sr);
        if (!data) {
            return false;
        }
        return data->fLength > 0;
    }

    static std::string GetDefaultChunkNbtFileName(int chunkX, int chunkZ) {
        std::ostringstream s;
        s << "c." << chunkX << "." << chunkZ << ".nbt";
        return s.str();
    }

    static std::string GetDefaultCompressedChunkNbtFileName(int chunkX, int chunkZ) {
        std::ostringstream s;
        s << "c." << chunkX << "." << chunkZ << ".nbt.z";
        return s.str();
    }
    
    static std::string GetDefaultRegionFileName(int regionX, int regionZ) {
        std::ostringstream s;
        s << "r." << regionX << "." << regionZ << ".mca";
        return s.str();
    }

    bool exportAllToNbt(std::string const& directory, std::function<std::string(int, int)> name = Region::GetDefaultChunkNbtFileName) const {
        int const minX = minChunkX();
        int const minZ = minChunkZ();
        int const maxX = maxChunkX();
        int const maxZ = maxChunkZ();
        for (int z = minZ; z <= maxZ; z++) {
            for (int x = minX; x <= maxX; x++) {
                std::string const n = name(x, z);
                std::string const path = directory + "/" + n;
                if (!exportToNbt(x, z, path)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool exportAllToCompressedNbt(std::string const& directory, std::function<std::string(int, int)> name = Region::GetDefaultCompressedChunkNbtFileName) const {
        int const minX = minChunkX();
        int const minZ = minChunkZ();
        int const maxX = maxChunkX();
        int const maxZ = maxChunkZ();
        for (int z = minZ; z <= maxZ; z++) {
            for (int x = minX; x <= maxX; x++) {
                std::string const n = name(x, z);
                std::string const path = directory + "/" + n;
                if (!exportToCompressedNbt(x, z, path)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool exportToNbt(int chunkX, int chunkZ, std::string const& filePath) const {
        int const localChunkX = chunkX - fX * 32;
        int const localChunkZ = chunkZ - fZ * 32;
        if (localChunkX < 0 || 32 <= localChunkX) {
            return false;
        }
        if (localChunkZ < 0 || 32 <= localChunkZ) {
            return false;
        }

        FILE *in = fopen(fFilePath.c_str(), "rb");
        if (!in) {
            return false;
        }
        int const index = (localChunkX & 31) + (localChunkZ & 31) * 32;
        if (fseek(in, 4 * index, SEEK_SET) != 0) {
            fclose(in);
            return false;
        }
        uint32_t loc;
        if (fread(&loc, sizeof(loc), 1, in) != 1) {
            fclose(in);
            return false;
        }
        loc = detail::Int32FromBE(loc);
        if (loc == 0) {
            fclose(in);
            return true;
        }
        long const sectorOffset = loc >> 8;
        if (fseek(in, 4 * index, SEEK_SET) != 0) {
            fclose(in);
            return false;
        }
        if (fseek(in, sectorOffset * kSectorSize, SEEK_SET) != 0) {
            fclose(in);
            return false;
        }
        uint32_t chunkSize;
        if (fread(&chunkSize, sizeof(chunkSize), 1, in) != 1) {
            fclose(in);
            return false;
        }
        chunkSize = detail::Int32FromBE(chunkSize) - 1;
        
        uint8_t compressionType;
        if (fread(&compressionType, sizeof(compressionType), 1, in) != 1) {
            fclose(in);
            return false;
        }
        if (compressionType != 2) {
            fclose(in);
            return false;
        }

        FILE* out = fopen(filePath.c_str(), "wb");
        if (!out) {
            fclose(in);
            return false;
        }
        
        std::vector<uint8_t> buffer(chunkSize);
        if (fread(buffer.data(), chunkSize, 1, in) != 1) {
            fclose(in);
            fclose(out);
            return false;
        }
        if (!detail::Compression::decompress(buffer)) {
            fclose(in);
            fclose(out);
            return false;
        }
        if (fwrite(buffer.data(), buffer.size(), 1, out) != 1) {
            fclose(in);
            fclose(out);
            return false;
        }
        
        fclose(in);
        fclose(out);
        
        return true;
    }

    bool exportToCompressedNbt(int chunkX, int chunkZ, std::string const& filePath) const {
        int const localChunkX = chunkX - fX * 32;
        int const localChunkZ = chunkZ - fZ * 32;
        if (localChunkX < 0 || 32 <= localChunkX) {
            return false;
        }
        if (localChunkZ < 0 || 32 <= localChunkZ) {
            return false;
        }

        FILE *in = fopen(fFilePath.c_str(), "rb");
        if (!in) {
            return false;
        }
        int const index = (localChunkX & 31) + (localChunkZ & 31) * 32;
        if (fseek(in, 4 * index, SEEK_SET) != 0) {
            fclose(in);
            return false;
        }
        uint32_t loc;
        if (fread(&loc, sizeof(loc), 1, in) != 1) {
            fclose(in);
            return false;
        }
        loc = detail::Int32FromBE(loc);
        if (loc == 0) {
            fclose(in);
            return true;
        }
        
        long const sectorOffset = loc >> 8;
        if (fseek(in, sectorOffset * kSectorSize, SEEK_SET) != 0) {
            fclose(in);
            return false;
        }
        uint32_t chunkSize;
        if (fread(&chunkSize, sizeof(chunkSize), 1, in) != 1) {
            fclose(in);
            return false;
        }
        chunkSize = detail::Int32FromBE(chunkSize) - 1;

        uint8_t compressionType;
        if (fread(&compressionType, sizeof(compressionType), 1, in) != 1) {
            fclose(in);
            return false;
        }

        FILE* out = fopen(filePath.c_str(), "wb");
        if (!out) {
            fclose(in);
            return false;
        }

        if (!stream::FileInputStream::Copy(in, out, chunkSize)) {
            fclose(in);
            fclose(out);
            return false;
        }
        
        fclose(in);
        fclose(out);

        return true;
    }

    static bool ConcatCompressedNbt(int regionX, int regionZ, std::string const& directory, std::string const& resultMcaFilePath, std::function<std::string(int chunkX, int chunkZ)> name = Region::GetDefaultCompressedChunkNbtFileName) {
        namespace fs = std::filesystem;

        int const minChunkX = regionX * 32;
        int const maxChunkX = minChunkX + 31;
        int const minChunkZ = regionZ * 32;
        int const maxChunkZ = minChunkZ + 31;
        bool exists = false;
        for (int z = minChunkZ; z <= maxChunkZ; z++) {
            for (int x = minChunkX; x <= maxChunkX; x++) {
                fs::path const filepath = fs::path(directory).append(name(x, z));
                std::error_code ec;
                if (fs::exists(filepath, ec)) {
                    exists = true;
                    break;
                }
            }
        }
        if (!exists) {
            return true;
        }

        FILE* out = fopen(resultMcaFilePath.c_str(), "wb");
        if (!out) {
            return false;
        }
        fseek(out, 2 * kSectorSize, SEEK_SET);
        
        std::vector<uint32_t> locationLut(1024, 0);
        for (int z = minChunkZ; z <= maxChunkZ; z++) {
            int const localChunkZ = z - minChunkZ;
            for (int x = minChunkX; x <= maxChunkX; x++) {
                int const localChunkX = x - minChunkX;
                int const index = (localChunkX & 31) + (localChunkZ & 31) * 32;
                std::string const filepath = directory + "/" + name(x, z);
                FILE *in = fopen(filepath.c_str(), "rb");
                if (!in) {
                    continue;
                }
                long const currentLocation = ftell(out);
                long const location = Ceil(currentLocation, kSectorSize);
                if (fseek(out, location, SEEK_SET) != 0) {
                    fclose(in);
                    fclose(out);
                    return false;
                }
                size_t const size = fs::file_size(filepath);
                uint8_t compressionType = 2;
                uint32_t s = detail::Int32BEFromNative(size + sizeof(compressionType));
                if (fwrite(&s, sizeof(s), 1, out) != 1) {
                    fclose(in);
                    fclose(out);
                    return false;
                }
                if (fwrite(&compressionType, sizeof(compressionType), 1, out) != 1) {
                    fclose(in);
                    fclose(out);
                    return false;
                }
                if (!stream::FileInputStream::Copy(in, out, size)) {
                    fclose(in);
                    fclose(out);
                    return false;
                }
                fclose(in);

                size_t headerSize = sizeof(s) + sizeof(compressionType);
                uint32_t const numSectors = Ceil(size + headerSize, kSectorSize) / kSectorSize;
                uint32_t const loc = (((((uint32_t)location) >> 12) << 8) & 0xFFFFFF00) | (numSectors & 0xFF);
                locationLut[index] = detail::Int32BEFromNative(loc);
            }
        }
        long const current = ftell(out);
        long const numZeroFill = Ceil(current, kSectorSize) - current;
        std::vector<uint8_t> zerofill(numZeroFill);
        if (fwrite(zerofill.data(), sizeof(uint8_t), numZeroFill, out) != numZeroFill) {
            fclose(out);
            return false;
        }
        
        if (fseek(out, 0, SEEK_SET) != 0) {
            fclose(out);
            return false;
        }
        if (fwrite(locationLut.data(), sizeof(uint32_t), locationLut.size(), out) != locationLut.size()) {
            fclose(out);
            return false;
        }
        std::vector<uint32_t> timestamp(1024, std::time(nullptr));
        if (fwrite(timestamp.data(), sizeof(uint32_t), timestamp.size(), out) != timestamp.size()) {
            fclose(out);
            return false;
        }
        fclose(out);
        return true;
    }
    
    static bool IterateRegionForCompressedNbtFiles(std::string const& chunkFilesDirectory,
                                                   std::function<bool(int regionX, int regionZ, std::string const& chunkFilesDirectory)> callback,
                                                   std::function<std::string(int regionX, int regionZ)> regionFileName = Region::GetDefaultRegionFileName) {
        namespace fs = std::filesystem;

        int minRegionX = 1;
        int maxRegionX = -1;
        int minRegionZ = 1;
        int maxRegionZ = -1;
        std::error_code err;
        for (auto& p : fs::directory_iterator(chunkFilesDirectory)) {
            if (!fs::is_regular_file(p, err)) {
                continue;
            }
            std::string name = p.path().filename().string();
            int chunkX;
            int chunkZ;
            if (sscanf(name.c_str(), "c.%d.%d.nbt.z", &chunkX, &chunkZ) != 2) {
                continue;
            }
            int const regionX = Coordinate::RegionFromChunk(chunkX);
            int const regionZ = Coordinate::RegionFromChunk(chunkZ);
            if (minRegionX > maxRegionX) {
                minRegionX = regionX;
                maxRegionX = regionX;
            } else {
                minRegionX = std::min(minRegionX, regionX);
                maxRegionX = std::max(maxRegionX, regionX);
            }
            if (minRegionZ > maxRegionZ) {
                minRegionZ = regionZ;
                maxRegionZ = regionZ;
            } else {
                minRegionZ = std::min(minRegionZ, regionZ);
                maxRegionZ = std::max(maxRegionZ, regionZ);
            }
        }
        if (minRegionX > maxRegionX || minRegionZ > maxRegionZ) {
            return true;
        }
        for (int x = minRegionX; x <= maxRegionX; x++) {
            for (int z = minRegionZ; z <= maxRegionZ; z++) {
                if (!callback(x, z, chunkFilesDirectory)) {
                    return false;
                }
            }
        }
        return true;
    }

private:
    Region(std::string const& filePath, int x, int z)
        : fX(x)
        , fZ(z)
        , fFilePath(filePath)
    {
    }

    std::shared_ptr<detail::McaDataSource> dataSource(int localChunkX, int localChunkZ, stream::InputStreamReader& sr) const {
        int const index = (localChunkX & 31) + (localChunkZ & 31) * 32;
        if (!sr.valid()) {
            return nullptr;
        }
        if (!sr.seek(4 * index)) {
            return nullptr;
        }

        uint32_t loc;
        if (!sr.read(&loc)) {
            return nullptr;
        }

        long sectorOffset = loc >> 8;
        if (!sr.seek(kSectorSize + 4 * index)) {
            return nullptr;
        }

        uint32_t timestamp;
        if (!sr.read(&timestamp)) {
            return nullptr;
        }

        if (!sr.seek(sectorOffset * kSectorSize)) {
            return nullptr;
        }
        uint32_t chunkSize;
        if (!sr.read(&chunkSize)) {
            return nullptr;
        }

        int const chunkX = this->fX * 32 + localChunkX;
        int const chunkZ = this->fZ * 32 + localChunkZ;
        return std::make_shared<detail::McaDataSource>(chunkX, chunkZ, timestamp, sectorOffset * kSectorSize, chunkSize);
    }
    
    bool loadChunkImpl(int regionX, int regionZ, stream::InputStreamReader& sr, bool& error, LoadChunkCallback callback) const {
        auto data = dataSource(regionX, regionZ, sr);
        if (!data) {
            error = true;
            return true;
        }
        if (data->loadChunk(sr, callback)) {
            return true;
        } else {
            error = true;
            return true;
        }
    }

    static uint32_t Ceil(uint32_t v, uint32_t div) {
        if (v % div == 0) {
            return v;
        }
        return v + (div - (v % div));
    }
    
public:
    int const fX;
    int const fZ;

private:
    std::string const fFilePath;

    static long const kSectorSize = 4096;
};

} // namespace mcfile
