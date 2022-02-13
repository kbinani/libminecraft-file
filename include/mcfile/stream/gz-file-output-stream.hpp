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

    ~GzFileOutputStream() {
        if (fFile) {
            gzclose(fFile);
        }
    }

    GzFileOutputStream(FileOutputStream const &) = delete;

    GzFileOutputStream &operator=(FileOutputStream const &) = delete;

    bool write(void const *buffer, size_t size) override {
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
