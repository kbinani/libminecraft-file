#pragma once

namespace mcfile {
namespace stream {

class FileInputStream : public InputStream {
public:
    explicit FileInputStream(std::filesystem::path const &filePath)
        : fFile(nullptr)
        , fLength(0)
        , fLoc(0) {
        FILE *fp = File::Open(filePath, File::Mode::Read);

        if (!fp) {
            return;
        }
        if (!File::Fseek(fp, 0, SEEK_END)) {
            fclose(fp);
            return;
        }
        std::optional<uint64_t> length = File::Ftell(fp);
        if (!length) {
            fclose(fp);
            return;
        }
        if (!File::Fseek(fp, 0, SEEK_SET)) {
            fclose(fp);
            return;
        }
        this->fFile = fp;
        this->fLength = *length;
    }

    FileInputStream(std::string const &) = delete;
    FileInputStream(std::wstring const &) = delete;

    ~FileInputStream() {
        if (fFile) {
            fclose(fFile);
        }
    }

    FileInputStream(FileInputStream const &) = delete;

    FileInputStream &operator=(FileInputStream const &) = delete;

    bool read(void *buffer, size_t size, size_t count) override {
        if (!fFile) {
            return false;
        }
        if (size == 0 || count == 0) {
            return true;
        }
        fLoc += size * count;
        return fread(buffer, size, count, fFile) == count;
    }

    bool seek(uint64_t offset) override {
        if (!fFile) {
            return false;
        }
        fLoc = offset;
        return File::Fseek(fFile, offset, SEEK_SET);
    }

    bool valid() const override {
        return fFile != nullptr;
    }

    uint64_t length() const override { return fLength; }

    uint64_t pos() const override { return fLoc; }

private:
    FILE *fFile;
    uint64_t fLength;
    uint64_t fLoc;
};

} // namespace stream
} // namespace mcfile
