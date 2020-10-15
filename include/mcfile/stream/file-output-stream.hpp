#pragma once

namespace mcfile {
namespace stream {

class FileOutputStream : public OutputStream {
public:
    explicit FileOutputStream(std::string const &filePath)
        : fFile(nullptr) {
        FILE *fp = fopen(filePath.c_str(), "wb");

        if (!fp) {
            return;
        }
        this->fFile = fp;
    }

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

    bool seek(long offset) override {
        return fseek(fFile, offset, SEEK_SET) == 0;
    }

    long pos() const override {
        return ftell(fFile);
    }

private:
    FILE *fFile;
};

} // namespace stream
} // namespace mcfile
