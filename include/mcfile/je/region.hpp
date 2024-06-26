#pragma once

namespace mcfile::je {

class Region {
public:
    Region(Region const &other)
        : fX(other.fX)
        , fZ(other.fZ)
        , fFilePath(other.fFilePath) {}

    Region &operator=(Region const &rh) = delete;

    struct LoadAllOptions {
        bool freadAtOnce = false;
    };
    bool loadAllChunks(std::function<bool(Chunk &)> callback, LoadAllOptions options = {.freadAtOnce = false}) const {
        std::shared_ptr<stream::InputStream> s;
        auto fs = std::make_shared<stream::FileInputStream>(fFilePath);
        std::vector<uint8_t> buffer;
        if (options.freadAtOnce) {
            if (!fs->valid()) {
                return false;
            }
            stream::InputStream::ReadUntilEos(*fs, buffer);
            fs.reset();
            s = std::make_shared<stream::ByteInputStream>((char const *)buffer.data(), buffer.size());
        } else {
            s = fs;
        }
        stream::InputStreamReader sr(s);
        for (int z = 0; z < 32; z++) {
            for (int x = 0; x < 32; x++) {
                if (!loadChunkImpl(x, z, sr, callback)) {
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
        std::shared_ptr<McaChunkLocator> src;
        bool ok = ChunkLocator(fX, fZ, localChunkX, localChunkZ, sr, src);
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
        std::shared_ptr<McaChunkLocator> src;
        bool ok = ChunkLocator(fX, fZ, localChunkX, localChunkZ, sr, src);
        if (!ok) {
            return nullptr;
        }
        if (!src) {
            return nullptr;
        }
        return src->loadWritableChunk(sr);
    }

    std::filesystem::path entitiesRegionFilePath() const {
        auto name = fFilePath.filename();
        auto path = std::filesystem::path(fFilePath).remove_filename().parent_path().parent_path().append("entities") / name;
        return path;
    }

    bool entitiesAt(int chunkX, int chunkZ, std::vector<std::shared_ptr<nbt::CompoundTag>> &buffer) const {
        namespace fs = std::filesystem;
        int const localChunkX = chunkX - fX * 32;
        int const localChunkZ = chunkZ - fZ * 32;
        if (localChunkX < 0 || 32 <= localChunkX || localChunkZ < 0 || 32 <= localChunkZ) {
            return false;
        }
        auto path = entitiesRegionFilePath();
        std::error_code ec;
        if (!fs::is_regular_file(path, ec)) {
            return true;
        }
        auto fin = std::make_shared<stream::FileInputStream>(path);
        stream::InputStreamReader sr(fin);
        std::shared_ptr<McaChunkLocator> src;
        bool ok = ChunkLocator(fX, fZ, localChunkX, localChunkZ, sr, src);
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
        auto entities = root->listTag(u8"Entities");
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

        return Clear(fFilePath, fX, fZ, localChunkX, localChunkZ);
    }

    bool clearEntities(int chunkX, int chunkZ) {
        int const localChunkX = chunkX - fX * 32;
        int const localChunkZ = chunkZ - fZ * 32;
        if (localChunkX < 0 || 32 <= localChunkX) {
            return false;
        }
        if (localChunkZ < 0 || 32 <= localChunkZ) {
            return false;
        }

        auto file = entitiesRegionFilePath();
        return Clear(file, fX, fZ, localChunkX, localChunkZ);
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
        std::shared_ptr<McaChunkLocator> data;
        bool ok = ChunkLocator(fX, fZ, localChunkX, localChunkZ, sr, data);
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
        auto type = RegionCompression::CompressionTypeFromUint8(compressionType);
        if (!type) {
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
        if (!Compression::DecompressZlib(buffer)) {
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

    std::shared_ptr<mcfile::nbt::CompoundTag> exportToNbt(int chunkX, int chunkZ) const {
        auto s = std::make_shared<mcfile::stream::FileInputStream>(fFilePath);
        mcfile::stream::InputStreamReader sr(s);
        if (!sr.valid()) {
            return nullptr;
        }
        std::shared_ptr<McaChunkLocator> locator;
        int x = chunkX - fX * 32;
        int z = chunkZ - fZ * 32;
        if (!ChunkLocator(fX, fZ, x, z, sr, locator)) {
            return nullptr;
        }
        if (!locator) {
            return nullptr;
        }
        return locator->load(sr);
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
        std::function<bool(int chunks)> fProgress = nullptr;
    };

    static bool ConcatCompressedNbt(int regionX, int regionZ, std::string const &directory, std::string const &resultMcaFilePath, ConcatOptions options = ConcatOptions()) = delete;

    static bool ConcatCompressedNbt(
        int regionX,
        int regionZ,
        std::filesystem::path const &directory,
        std::filesystem::path const &resultMcaFilePath,
        ConcatOptions options = ConcatOptions()) {
        using namespace std;
        namespace fs = std::filesystem;

        auto output = make_shared<mcfile::stream::DeferOpeningOutputStream>([resultMcaFilePath]() { return make_shared<mcfile::stream::FileOutputStream>(resultMcaFilePath); });
        int chunks = 0;
        return SquashChunksAsMca(*output, [regionX, regionZ, options, directory, &chunks](int localChunkX, int localChunkZ, mcfile::stream::OutputStream &stream, bool &stop) {
            int cx = regionX * 32 + localChunkX;
            int cz = regionZ * 32 + localChunkZ;
            string filename = options.fChunkFileName(cx, cz);
            auto file = fs::path(directory) / filename;
            error_code ec;
            if (!fs::is_regular_file(file, ec)) {
                chunks++;
                if (options.fProgress && !options.fProgress(chunks)) {
                    stop = true;
                }
                return;
            }
            if (ec) {
                stop = true;
                return;
            }
            auto input = make_shared<mcfile::stream::FileInputStream>(file);
            vector<uint8_t> buffer(512);
            while (input->valid()) {
                size_t read = input->read(buffer.data(), buffer.size());
                if (!stream.write(buffer.data(), read)) {
                    stop = true;
                    return;
                }
            }
            chunks++;
            if (options.fProgress && !options.fProgress(chunks)) {
                stop = true;
            }
        });
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
            if (!mcfile::nbt::CompoundTag::WriteDeflateCompressed(*tag, out, Encoding::Java)) {
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

    bool loadChunkImpl(int regionX, int regionZ, stream::InputStreamReader &sr, std::function<bool(Chunk &)> callback) const {
        std::shared_ptr<McaChunkLocator> data;
        bool ok = ChunkLocator(fX, fZ, regionX, regionZ, sr, data);
        if (!ok) {
            return false;
        }
        if (!data) {
            // chunk not saved yet
            return true;
        }
        if (data->loadChunk(sr, callback)) {
            return true;
        } else {
            return false;
        }
    }

    static uint32_t Ceil(uint32_t v, uint32_t div) {
        if (v % div == 0) {
            return v;
        }
        return v + (div - (v % div));
    }

    static bool ChunkLocator(int regionX, int regionZ, int localChunkX, int localChunkZ, stream::InputStreamReader &sr, std::shared_ptr<McaChunkLocator> &out) {
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

        int const chunkX = regionX * 32 + localChunkX;
        int const chunkZ = regionZ * 32 + localChunkZ;
        out.reset(new McaChunkLocator(chunkX, chunkZ, timestamp, sectorOffset * kSectorSize, chunkSize));
        return true;
    }

    static bool Clear(std::filesystem::path const &file, int regionX, int regionZ, int localChunkX, int localChunkZ) {
        if (localChunkX < 0 || 32 <= localChunkX) {
            return false;
        }
        if (localChunkZ < 0 || 32 <= localChunkZ) {
            return false;
        }

        std::shared_ptr<McaChunkLocator> data;
        {
            auto fs = std::make_shared<stream::FileInputStream>(file);
            stream::InputStreamReader sr(fs);
            if (!ChunkLocator(regionX, regionZ, localChunkX, localChunkZ, sr, data)) {
                return true;
            }
            if (!data) {
                return false;
            }
            fs.reset();
        }

        ScopedFile fp(File::Open(file, File::Mode::ReadWrite));
        if (!fp) {
            return false;
        }
        uint64_t index = localChunkX + localChunkZ * 32;
        if (!File::Fseek(fp.get(), index * 4, SEEK_SET)) {
            return false;
        }
        uint32_t zero = 0;
        if (!File::Fwrite(&zero, sizeof(zero), 1, fp.get())) {
            return false;
        }
        if (!File::Fseek(fp.get(), data->fOffset, SEEK_SET)) {
            return false;
        }
        if (!File::Fwrite(&zero, sizeof(zero), 1, fp.get())) {
            return false;
        }
        uint8_t clear[128] = {0};
        uint64_t remains = data->fLength;
        while (remains > 0) {
            uint64_t amount = remains > 128 ? 128 : remains;
            if (!File::Fwrite(clear, amount, 1, fp.get())) {
                return false;
            }
            remains -= amount;
        }

        return true;
    }

public:
    int const fX;
    int const fZ;
    std::filesystem::path const fFilePath;

private:
    static uint64_t const kSectorSize = 4096;
};

} // namespace mcfile::je
