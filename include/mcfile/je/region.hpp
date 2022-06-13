#pragma once

namespace mcfile::je {

class Region {
public:
    Region(Region const &other)
        : fX(other.fX)
        , fZ(other.fZ)
        , fFilePath(other.fFilePath) {}

    Region &operator=(Region const &rh) = delete;

    using LoadChunkCallback = std::function<bool(Chunk &)>;

    bool loadAllChunks(bool &error, LoadChunkCallback callback) const {
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
        std::shared_ptr<McaDataSource> src;
        bool ok = dataSource(localChunkX, localChunkZ, sr, src);
        if (!ok) {
            return nullptr;
        }
        if (!src) {
            return nullptr;
        }
        return src->loadChunk(sr);
    }

    std::shared_ptr<WritableChunk> writableChunkAt(int chunkX, int chunkZ) const {
        int const localChunkX = chunkX - fX * 32;
        int const localChunkZ = chunkZ - fZ * 32;
        if (localChunkX < 0 || 32 <= localChunkX || localChunkZ < 0 || 32 <= localChunkZ) {
            return nullptr;
        }
        auto fs = std::make_shared<stream::FileInputStream>(fFilePath);
        stream::InputStreamReader sr(fs);
        std::shared_ptr<McaDataSource> src;
        bool ok = dataSource(localChunkX, localChunkZ, sr, src);
        if (!ok) {
            return nullptr;
        }
        if (!src) {
            return nullptr;
        }
        return src->loadWritableChunk(sr);
    }

    bool entitiesAt(int chunkX, int chunkZ, std::vector<std::shared_ptr<nbt::CompoundTag>> &buffer) const {
        namespace fs = std::filesystem;
        int const localChunkX = chunkX - fX * 32;
        int const localChunkZ = chunkZ - fZ * 32;
        if (localChunkX < 0 || 32 <= localChunkX || localChunkZ < 0 || 32 <= localChunkZ) {
            return false;
        }
        std::string name = fFilePath.filename().string();
        auto path = fs::path(fFilePath).remove_filename().parent_path().parent_path().append("entities").append(name);
        std::error_code ec;
        if (!fs::is_regular_file(path, ec)) {
            return true;
        }
        auto fin = std::make_shared<stream::FileInputStream>(path);
        stream::InputStreamReader sr(fin);
        std::shared_ptr<McaDataSource> src;
        bool ok = dataSource(localChunkX, localChunkZ, sr, src);
        if (!ok) {
            return false;
        }
        if (!src) {
            // entity not saved yet
            return true;
        }
        auto root = src->load(sr);
        if (!root) {
            return false;
        }
        auto entities = root->listTag("Entities");
        if (!entities) {
            return false;
        }
        buffer.clear();
        for (auto const &e : entities->fValue) {
            if (e->type() != nbt::Tag::Type::Compound) {
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

    static std::shared_ptr<Region> MakeRegion(std::filesystem::path const &filePath, int x, int z) {
        return std::shared_ptr<Region>(new Region(filePath, x, z));
    }

    static std::optional<Pos2i> RegionXZFromFile(std::filesystem::path const &path) {
        using namespace std;
        namespace fs = std::filesystem;
        string basename = path.filename().string();
        if (basename.size() < 9) {
            return nullopt;
        }
        // r.x.z.mca
        if (basename[0] != 'r' || basename[1] != '.') {
            return nullopt;
        }
        auto secondDot = basename.find('.', 2);
        if (secondDot == string::npos) {
            return nullopt;
        }
        auto thirdDot = basename.find('.', secondDot + 1);
        if (thirdDot == string::npos) {
            return nullopt;
        }
        auto trailing = basename.substr(thirdDot);
        if (trailing != ".mca") {
            return nullopt;
        }
        string rxStr;
        string rzStr;
        rxStr.assign(basename.begin() + 2, basename.begin() + secondDot);
        rzStr.assign(basename.begin() + secondDot + 1, basename.begin() + thirdDot);
        int rx;
        int rz;
        try {
            rx = stoi(rxStr);
            rz = stoi(rzStr);
        } catch (...) {
            return nullopt;
        }
        return Pos2i(rx, rz);
    }

    static std::shared_ptr<Region> MakeRegion(std::string const &, int, int) = delete;

    static std::shared_ptr<Region> MakeRegion(std::filesystem::path const &filePath) {
        auto xz = RegionXZFromFile(filePath);
        if (!xz) {
            return nullptr;
        }

        return std::shared_ptr<Region>(new Region(filePath, xz->fX, xz->fZ));
    }

    static std::shared_ptr<Region> MakeRegion(std::string const &) = delete;

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

        FILE *fp = File::Open(fFilePath, File::Mode::Read);
        if (!fp) {
            return false;
        }
        int const index = (localChunkX & 31) + (localChunkZ & 31) * 32;
        if (!File::Fseek(fp, 4 * index, SEEK_SET)) {
            fclose(fp);
            return false;
        }
        uint32_t loc;
        if (!File::Fread(&loc, sizeof(loc), 1, fp)) {
            fclose(fp);
            return false;
        }
        loc = U32FromBE(loc);
        if (loc == 0) {
            fclose(fp);
            return true;
        }
        long const sectorOffset = loc >> 8;
        if (!File::Fseek(fp, 4 * index, SEEK_SET)) {
            fclose(fp);
            return false;
        }
        loc = 0;
        if (!File::Fwrite(&loc, sizeof(loc), 1, fp)) {
            fclose(fp);
            return false;
        }

        if (!File::Fseek(fp, kSectorSize + 4 * index, SEEK_SET)) {
            fclose(fp);
            return false;
        }
        uint32_t timestamp = 0;
        if (!File::Fwrite(&timestamp, sizeof(timestamp), 1, fp)) {
            fclose(fp);
            return false;
        }
        if (!File::Fseek(fp, sectorOffset * kSectorSize, SEEK_SET)) {
            fclose(fp);
            return false;
        }
        uint32_t chunkSize;
        if (!File::Fread(&chunkSize, sizeof(chunkSize), 1, fp)) {
            fclose(fp);
            return false;
        }
        chunkSize = U32FromBE(chunkSize);
        for (uint32_t i = 0; i < chunkSize; i++) {
            uint8_t zero = 0;
            if (!File::Fwrite(&zero, sizeof(zero), 1, fp)) {
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
        std::shared_ptr<McaDataSource> data;
        bool ok = dataSource(localChunkX, localChunkZ, sr, data);
        if (!ok) {
            return false;
        }
        if (!data) {
            return false;
        }
        return true;
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

    bool exportAllToNbt(std::string const &directory, std::function<std::string(int, int)> name = Region::GetDefaultChunkNbtFileName) const = delete;

    bool exportAllToNbt(std::filesystem::path const &directory, std::function<std::string(int, int)> name = Region::GetDefaultChunkNbtFileName) const {
        int const minX = minChunkX();
        int const minZ = minChunkZ();
        int const maxX = maxChunkX();
        int const maxZ = maxChunkZ();
        for (int z = minZ; z <= maxZ; z++) {
            for (int x = minX; x <= maxX; x++) {
                std::string const n = name(x, z);
                auto path = directory / n;
                if (!exportToNbt(x, z, path)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool exportAllToCompressedNbt(std::string const &directory, std::function<std::string(int, int)> name = Region::GetDefaultCompressedChunkNbtFileName) const = delete;

    bool exportAllToCompressedNbt(std::filesystem::path const &directory, std::function<std::string(int, int)> name = Region::GetDefaultCompressedChunkNbtFileName) const {
        int const minX = minChunkX();
        int const minZ = minChunkZ();
        int const maxX = maxChunkX();
        int const maxZ = maxChunkZ();
        for (int z = minZ; z <= maxZ; z++) {
            for (int x = minX; x <= maxX; x++) {
                std::string const n = name(x, z);
                auto path = directory / n;
                if (!exportToCompressedNbt(x, z, path)) {
                    return false;
                }
            }
        }
        return true;
    }

    bool exportToNbt(int chunkX, int chunkZ, std::string const &filePath) const = delete;

    bool exportToNbt(int chunkX, int chunkZ, std::filesystem::path const &filePath) const {
        int const localChunkX = chunkX - fX * 32;
        int const localChunkZ = chunkZ - fZ * 32;
        if (localChunkX < 0 || 32 <= localChunkX) {
            return false;
        }
        if (localChunkZ < 0 || 32 <= localChunkZ) {
            return false;
        }

        FILE *in = File::Open(fFilePath, File::Mode::Read);
        if (!in) {
            return false;
        }
        int const index = (localChunkX & 31) + (localChunkZ & 31) * 32;
        if (!File::Fseek(in, 4 * index, SEEK_SET)) {
            fclose(in);
            return false;
        }
        uint32_t loc;
        if (!File::Fread(&loc, sizeof(loc), 1, in)) {
            fclose(in);
            return false;
        }
        loc = U32FromBE(loc);
        if (loc == 0) {
            fclose(in);
            // The chunk is not saved yet
            return true;
        }
        long const sectorOffset = loc >> 8;
        if (!File::Fseek(in, 4 * index, SEEK_SET)) {
            fclose(in);
            return false;
        }
        if (!File::Fseek(in, sectorOffset * kSectorSize, SEEK_SET)) {
            fclose(in);
            return false;
        }
        uint32_t size;
        if (!File::Fread(&size, sizeof(size), 1, in)) {
            fclose(in);
            return false;
        }
        size = U32FromBE(size);
        if (size == 0) {
            fclose(in);
            // The chunk is not saved yet
            return true;
        }
        uint32_t const chunkSize = size - 1;

        uint8_t compressionType;
        if (!File::Fread(&compressionType, sizeof(compressionType), 1, in)) {
            fclose(in);
            return false;
        }
        if (compressionType != 2) {
            fclose(in);
            return false;
        }

        FILE *out = File::Open(filePath, File::Mode::Write);
        if (!out) {
            fclose(in);
            return false;
        }

        std::vector<uint8_t> buffer(chunkSize);
        if (!File::Fread(buffer.data(), chunkSize, 1, in)) {
            fclose(in);
            fclose(out);
            return false;
        }
        if (!Compression::Decompress(buffer)) {
            fclose(in);
            fclose(out);
            return false;
        }
        if (!File::Fwrite(buffer.data(), buffer.size(), 1, out)) {
            fclose(in);
            fclose(out);
            return false;
        }

        fclose(in);
        fclose(out);

        return true;
    }

    bool exportToCompressedNbt(int chunkX, int chunkZ, std::string const &filePath) const = delete;

    bool exportToCompressedNbt(int chunkX, int chunkZ, std::filesystem::path const &filePath) const {
        mcfile::stream::FileOutputStream output(filePath);
        return exportToCompressedNbt(chunkX, chunkZ, output);
    }

    bool exportToCompressedNbt(int chunkX, int chunkZ, mcfile::stream::OutputStream &output) const {
        int const localChunkX = chunkX - fX * 32;
        int const localChunkZ = chunkZ - fZ * 32;
        if (localChunkX < 0 || 32 <= localChunkX) {
            return false;
        }
        if (localChunkZ < 0 || 32 <= localChunkZ) {
            return false;
        }

        FILE *in = File::Open(fFilePath, File::Mode::Read);
        if (!in) {
            return false;
        }
        int const index = (localChunkX & 31) + (localChunkZ & 31) * 32;
        if (!File::Fseek(in, 4 * index, SEEK_SET)) {
            fclose(in);
            return false;
        }
        uint32_t loc;
        if (!File::Fread(&loc, sizeof(loc), 1, in)) {
            fclose(in);
            return false;
        }
        loc = U32FromBE(loc);
        if (loc == 0) {
            fclose(in);
            // The chunk is not saved yet
            return true;
        }

        long const sectorOffset = loc >> 8;
        if (!File::Fseek(in, sectorOffset * kSectorSize, SEEK_SET)) {
            fclose(in);
            return false;
        }
        uint32_t size;
        if (!File::Fread(&size, sizeof(size), 1, in)) {
            fclose(in);
            return false;
        }
        size = U32FromBE(size);
        if (size == 0) {
            fclose(in);
            // The chunk is not saved yet
            return true;
        }
        uint32_t const chunkSize = size - 1;

        uint8_t compressionType;
        if (!File::Fread(&compressionType, sizeof(compressionType), 1, in)) {
            fclose(in);
            return false;
        }

        uint8_t buffer[512] = {0};
        uint32_t remaining = chunkSize;
        while (remaining > 0) {
            uint32_t amount = (std::min)(remaining, (uint32_t)512);
            if (!File::Fread(buffer, amount, 1, in)) {
                fclose(in);
                return false;
            }
            if (!output.write(buffer, amount)) {
                fclose(in);
                return false;
            }
            remaining -= amount;
        }
        fclose(in);
        return true;
    }

    struct ConcatOptions {
        ConcatOptions()
            : fChunkFileName(Region::GetDefaultCompressedChunkNbtFileName)
            , fDeleteInput(false) {}
        std::function<std::string(int chunkX, int chunkZ)> fChunkFileName = Region::GetDefaultCompressedChunkNbtFileName;
        bool fDeleteInput = false;
    };

    static bool ConcatCompressedNbt(int regionX, int regionZ, std::string const &directory, std::string const &resultMcaFilePath, ConcatOptions options = ConcatOptions()) = delete;

    static bool ConcatCompressedNbt(int regionX, int regionZ, std::filesystem::path const &directory, std::filesystem::path const &resultMcaFilePath, ConcatOptions options = ConcatOptions()) {
        namespace fs = std::filesystem;

        int const minChunkX = regionX * 32;
        int const maxChunkX = minChunkX + 31;
        int const minChunkZ = regionZ * 32;
        int const maxChunkZ = minChunkZ + 31;
        bool exists = false;
        for (int z = minChunkZ; z <= maxChunkZ; z++) {
            for (int x = minChunkX; x <= maxChunkX; x++) {
                fs::path const filepath = fs::path(directory).append(options.fChunkFileName(x, z));
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

        FILE *out = File::Open(resultMcaFilePath, File::Mode::Write);
        if (!out) {
            return false;
        }
        if (!File::Fseek(out, 2 * kSectorSize, SEEK_SET)) {
            return false;
        }

        std::vector<uint32_t> locationLut(1024, 0);
        for (int z = minChunkZ; z <= maxChunkZ; z++) {
            int const localChunkZ = z - minChunkZ;
            for (int x = minChunkX; x <= maxChunkX; x++) {
                int const localChunkX = x - minChunkX;
                int const index = (localChunkX & 31) + (localChunkZ & 31) * 32;
                fs::path const filepath = directory / options.fChunkFileName(x, z);
                FILE *in = File::Open(filepath, File::Mode::Read);
                if (!in) {
                    continue;
                }
                long const currentLocation = ftell(out);
                long const location = Ceil(currentLocation, kSectorSize);
                if (!File::Fseek(out, location, SEEK_SET)) {
                    fclose(in);
                    fclose(out);
                    return false;
                }
                size_t const size = fs::file_size(filepath);
                uint8_t compressionType = 2;
                uint32_t s = U32BEFromNative(size + sizeof(compressionType));
                if (!File::Fwrite(&s, sizeof(s), 1, out)) {
                    fclose(in);
                    fclose(out);
                    return false;
                }
                if (!File::Fwrite(&compressionType, sizeof(compressionType), 1, out)) {
                    fclose(in);
                    fclose(out);
                    return false;
                }
                if (!File::Copy(in, out, size)) {
                    fclose(in);
                    fclose(out);
                    return false;
                }
                fclose(in);

                if (options.fDeleteInput) {
                    std::error_code ec;
                    fs::remove(filepath, ec);
                }

                size_t headerSize = sizeof(s) + sizeof(compressionType);
                uint32_t const numSectors = Ceil(size + headerSize, kSectorSize) / kSectorSize;
                uint32_t const loc = (((((uint32_t)location) >> 12) << 8) & 0xFFFFFF00) | (numSectors & 0xFF);
                locationLut[index] = U32BEFromNative(loc);
            }
        }
        long const current = ftell(out);
        long const numZeroFill = Ceil(current, kSectorSize) - current;
        std::vector<uint8_t> zerofill(numZeroFill);
        if (!File::Fwrite(zerofill.data(), sizeof(uint8_t), numZeroFill, out)) {
            fclose(out);
            return false;
        }

        if (!File::Fseek(out, 0, SEEK_SET)) {
            fclose(out);
            return false;
        }
        if (!File::Fwrite(locationLut.data(), sizeof(uint32_t), locationLut.size(), out)) {
            fclose(out);
            return false;
        }
        std::vector<uint32_t> timestamp(1024, std::time(nullptr));
        if (!File::Fwrite(timestamp.data(), sizeof(uint32_t), timestamp.size(), out)) {
            fclose(out);
            return false;
        }
        fclose(out);
        return true;
    }

    static bool SquashChunksAsMca(mcfile::stream::OutputStream &output,
                                  std::function<void(int localChunkX, int localChunkZ, mcfile::stream::OutputStream &output, bool &stop)> callback) {
        using namespace std;
        using namespace mcfile::stream;
        namespace fs = std::filesystem;

        map<uint64_t, uint32_t> deferredWrite;

        if (!output.seek(2048 * 4)) {
            return false;
        }

        int index = 0;
        for (int z = 0; z < 32; z++) {
            for (int x = 0; x < 32; x++, index++) {
                uint64_t pos = output.pos();
                uint32_t const sectionHead = Ceil(pos, kSectorSize);
                if (!output.seek(sectionHead + 4 + 1)) {
                    return false;
                }
                uint32_t const before = sectionHead + 4;

                bool stop = false;
                callback(x, z, output, stop);
                if (stop) {
                    return false;
                }
                uint64_t const after = output.pos();
                if (after == before + 1) {
                    // nothing written, so seek back to initial position
                    if (!output.seek(pos)) {
                        return false;
                    }
                    continue;
                }
                if (!output.seek(sectionHead)) {
                    return false;
                }
                uint32_t sizeValue = U32BEFromNative(after - before);
                if (!output.write(&sizeValue, sizeof(sizeValue))) {
                    return false;
                }
                uint8_t compressionType = 2;
                if (!output.write(&compressionType, 1)) {
                    return false;
                }
                if (!output.seek(after)) {
                    return false;
                }

                uint32_t const size = after - sectionHead;
                uint32_t const numSectors = Ceil(size, kSectorSize) / kSectorSize;
                uint32_t const loc = (((sectionHead >> 12) << 8) & 0xFFFFFF00) | (numSectors & 0xFF);
                deferredWrite[index * 4] = U32BEFromNative(loc);
                deferredWrite[4096 + index * 4] = U32BEFromNative(std::time(nullptr));
            }
        }

        uint64_t const current = output.pos();
        if (current == 2048 * 4) {
            if (!output.seek(0)) {
                return false;
            }
            return output.truncate();
        }

        uint64_t const numZeroFill = Ceil(current, kSectorSize) - current;
        vector<uint8_t> zerofill(numZeroFill);
        if (!output.write(zerofill.data(), numZeroFill)) {
            return false;
        }
        for (auto const &it : deferredWrite) {
            uint64_t pos = it.first;
            if (!output.seek(pos)) {
                return false;
            }
            uint32_t data = it.second;
            if (!output.write(&data, sizeof(data))) {
                return false;
            }
        }

        return true;
    }

    static bool SquashChunksAsMca(mcfile::stream::OutputStream &output,
                                  std::function<std::shared_ptr<mcfile::nbt::CompoundTag>(int localChunkX, int localChunkZ, bool &stop)> callback) {
        return SquashChunksAsMca(output, [&callback](int x, int z, mcfile::stream::OutputStream &out, bool &stop) {
            auto tag = callback(x, z, stop);
            if (stop) {
                return;
            }
            if (!tag) {
                return;
            }
            if (!mcfile::nbt::CompoundTag::WriteCompressed(*tag, out, Endian::Big)) {
                stop = true;
            }
        });
    }

    static bool IterateRegionForCompressedNbtFiles(std::string const &chunkFilesDirectory,
                                                   std::function<bool(int regionX, int regionZ, std::string const &chunkFilesDirectory)> callback,
                                                   std::function<std::string(int regionX, int regionZ)> regionFileName = Region::GetDefaultRegionFileName) {
        namespace fs = std::filesystem;

        int minRegionX = 1;
        int maxRegionX = -1;
        int minRegionZ = 1;
        int maxRegionZ = -1;
        std::error_code err;
        fs::directory_iterator itr(chunkFilesDirectory, err);
        if (err) {
            return false;
        }
        for (auto &p : itr) {
            err.clear();
            if (!fs::is_regular_file(p, err)) {
                continue;
            }
            if (err) {
                continue;
            }
            std::string name = p.path().filename().string();
            int chunkX;
            int chunkZ;
#if defined(_WIN32) || (__STDC_LIB_EXT1__ == 1 && __STDC_WANT_LIB_EXT1__ == 1)
            auto sscanf = sscanf_s;
#else
            auto sscanf = std::sscanf;
#endif
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
    Region(std::filesystem::path const &filePath, int x, int z)
        : fX(x)
        , fZ(z)
        , fFilePath(filePath) {
    }

    bool dataSource(int localChunkX, int localChunkZ, stream::InputStreamReader &sr, std::shared_ptr<McaDataSource> &out) const {
        out.reset();

        uint64_t const index = (localChunkX & 31) + (localChunkZ & 31) * 32;
        if (!sr.valid()) {
            return false;
        }
        if (!sr.seek(4 * index)) {
            return false;
        }

        uint32_t loc;
        if (!sr.read(&loc)) {
            return false;
        }
        if (loc == 0) {
            // chunk not saved yet
            return true;
        }

        uint64_t sectorOffset = loc >> 8;
        if (!sr.seek(kSectorSize + 4 * index)) {
            return false;
        }

        uint32_t timestamp;
        if (!sr.read(&timestamp)) {
            return false;
        }

        if (!sr.seek(sectorOffset * kSectorSize)) {
            return false;
        }
        uint32_t chunkSize;
        if (!sr.read(&chunkSize)) {
            return false;
        }
        if (chunkSize == 0) {
            // chunk not saved yet
            return true;
        }

        int const chunkX = this->fX * 32 + localChunkX;
        int const chunkZ = this->fZ * 32 + localChunkZ;
        out.reset(new McaDataSource(chunkX, chunkZ, timestamp, sectorOffset * kSectorSize, chunkSize));
        return true;
    }

    bool loadChunkImpl(int regionX, int regionZ, stream::InputStreamReader &sr, bool &error, LoadChunkCallback callback) const {
        std::shared_ptr<McaDataSource> data;
        bool ok = dataSource(regionX, regionZ, sr, data);
        if (!ok) {
            error = true;
            return true;
        }
        if (!data) {
            // chunk not saved yet
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
    std::filesystem::path const fFilePath;

private:
    static uint64_t const kSectorSize = 4096;
};

} // namespace mcfile::je
