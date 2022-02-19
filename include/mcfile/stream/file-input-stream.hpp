#pragma once

namespace mcfile::stream {

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
        size_t read = fread(buffer, size, count, fFile);
        fLoc += read * size;
        return read == count;
    }

    size_t read(void *buffer, size_t size) override {
        if (!fFile) {
            return 0;
        }
        if (size == 0) {
            return 0;
        }
        size_t read = fread(buffer, 1, size, fFile);
        fLoc += read;
        return read;
    }

    bool seek(uint64_t offset) override {
        if (!fFile) {
            return false;
        }
        fLoc = offset;
        return File::Fseek(fFile, offset, SEEK_SET);
    }

    bool valid() const override {
        if (!fFile) {
            return false;
        }
        return feof(fFile) == 0;
    }

    uint64_t pos() const override { return fLoc; }

private:
    FILE *fFile;
    uint64_t fLength;
    uint64_t fLoc;
};

} // namespace mcfile::stream
