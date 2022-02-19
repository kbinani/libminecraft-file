#pragma once

namespace mcfile::stream {

class GzFileInputStream : public InputStream {
public:
    explicit GzFileInputStream(std::filesystem::path const &filePath)
        : fFile(nullptr)
        , fPos(0) {
        gzFile fp = File::GzOpen(filePath, File::Mode::Read);
        if (!fp) {
            return;
        }
        fFile = fp;
    }

    GzFileInputStream(std::string const &) = delete;
    GzFileInputStream(std::wstring const &) = delete;

    ~GzFileInputStream() {
        if (fFile) {
            gzclose(fFile);
        }
    }

    GzFileInputStream(GzFileInputStream const &) = delete;

    GzFileInputStream &operator=(GzFileInputStream const &) = delete;

    bool read(void *buffer, size_t size, size_t count) override {
        if (!fFile) {
            return false;
        }
        if (size == 0 || count == 0) {
            return true;
        }
        fPos += size * count;
        return gzfread(buffer, size, count, fFile) == count;
    }

    bool seek(uint64_t offset) override {
        if (!fFile) {
            return false;
        }
        if (gzseek(fFile, offset, SEEK_SET) != -1) {
            fPos = offset;
            return true;
        } else {
            return false;
        }
    }

    uint64_t pos() const override {
        return fPos;
    }

private:
    gzFile fFile;
    uint64_t fPos = 0;
};

} // namespace mcfile::stream
