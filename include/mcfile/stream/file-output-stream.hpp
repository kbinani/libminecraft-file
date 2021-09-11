#pragma once

namespace mcfile {
namespace stream {

class FileOutputStream : public OutputStream {
public:
    explicit FileOutputStream(std::filesystem::path const &filePath)
        : fFile(nullptr) {
        FILE *fp = File::Open(filePath, File::Mode::Write);

        if (!fp) {
            return;
        }
        this->fFile = fp;
    }

    FileOutputStream(std::string const &) = delete;
    FileOutputStream(std::wstring const &) = delete;

    ~FileOutputStream() {
        if (fFile) {
            fclose(fFile);
        }
    }

    FileOutputStream(FileOutputStream const &) = delete;

    FileOutputStream &operator=(FileOutputStream const &) = delete;

    bool write(void *buffer, size_t size) override {
        return fwrite(buffer, size, 1, fFile) == 1;
    }

    bool seek(uint64_t offset) override {
        return File::Fseek(fFile, offset, SEEK_SET);
    }

    uint64_t pos() const override {
        return ftell(fFile);
    }

private:
    FILE *fFile;
};

} // namespace stream
} // namespace mcfile
