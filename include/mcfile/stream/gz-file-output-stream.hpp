#pragma once

namespace mcfile::stream {

class GzFileOutputStream : public OutputStream {
public:
    explicit GzFileOutputStream(std::filesystem::path const &filePath)
        : fFile(nullptr)
        , fPos(0) {
        gzFile fp = File::GzOpen(filePath, File::Mode::Write);
        if (!fp) {
            return;
        }
        fFile = fp;
    }

    GzFileOutputStream(std::string const &) = delete;
    GzFileOutputStream(std::wstring const &) = delete;

    ~GzFileOutputStream() override {
        if (fFile) {
            gzclose(fFile);
        }
    }

    GzFileOutputStream(GzFileOutputStream const &) = delete;

    GzFileOutputStream &operator=(GzFileOutputStream const &) = delete;

    bool write(void const *buffer, size_t size) override {
        if (!fFile) {
            return false;
        }
        if (size == 0) {
            return true;
        }
        if (gzwrite(fFile, buffer, size) == 0) {
            return false;
        } else {
            fPos += size;
            return true;
        }
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

    bool truncate() override {
        // unsupported operation
        return false;
    }

private:
    gzFile fFile;
    uint64_t fPos = 0;
};

} // namespace mcfile::stream
