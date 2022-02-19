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

    size_t read(void *buffer, size_t size) override {
        if (!fFile) {
            return 0;
        }
        if (size == 0) {
            return 0;
        }
        size_t read = gzfread(buffer, 1, size, fFile);
        fPos += read;
        return read;
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

    bool valid() const override {
        if (!fFile) {
            return false;
        }
        return gzeof(fFile) != 1;
    }

private:
    gzFile fFile;
    uint64_t fPos = 0;
};

} // namespace mcfile::stream
