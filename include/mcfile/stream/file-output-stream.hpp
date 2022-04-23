#pragma once

namespace mcfile {
namespace stream {

class FileOutputStream : public OutputStream {
public:
    explicit FileOutputStream(std::filesystem::path const &filePath)
        : fFile(nullptr)
        , fPos(0) {
        FILE *fp = File::Open(filePath, File::Mode::Write);

        if (!fp) {
            return;
        }
        this->fFile = fp;
    }

    FileOutputStream(std::string const &) = delete;
    FileOutputStream(std::wstring const &) = delete;

    ~FileOutputStream() override {
        if (fFile) {
            fclose(fFile);
        }
    }

    FileOutputStream(FileOutputStream const &) = delete;

    FileOutputStream &operator=(FileOutputStream const &) = delete;

    bool write(void const *buffer, size_t size) override {
        if (!fFile) {
            return false;
        }
        if (size == 0) {
            return true;
        }
        if (fwrite(buffer, size, 1, fFile) == 1) {
            fPos += size;
            return true;
        } else {
            return false;
        }
    }

    bool seek(uint64_t offset) override {
        if (!fFile) {
            return false;
        }
        if (File::Fseek(fFile, offset, SEEK_SET)) {
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
    FILE *fFile;
    uint64_t fPos = 0;
};

} // namespace stream
} // namespace mcfile
