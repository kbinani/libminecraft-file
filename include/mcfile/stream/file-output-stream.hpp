#pragma once

namespace mcfile {
namespace stream {

class FileOutputStream : public OutputStream {
public:
    explicit FileOutputStream(std::filesystem::path const &filePath, bool deferOpen = false)
        : fFile(nullptr)
        , fPos(0)
        , fPath(filePath) {
        if (!deferOpen) {
            ensureOpened();
        }
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
        FILE *file = ensureOpened();
        if (!file) {
            return false;
        }
        if (size == 0) {
            return true;
        }
        if (fwrite(buffer, size, 1, file) == 1) {
            fPos += size;
            return true;
        } else {
            return false;
        }
    }

    bool seek(uint64_t offset) override {
        FILE *file = ensureOpened();
        if (!file) {
            return false;
        }
        if (File::Fseek(file, offset, SEEK_SET)) {
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
        FILE *file = ensureOpened();
#if defined(_MSC_VER)
        return _chsize_s(fileno(file), fPos) == 0;
#else
        return ftruncate(fileno(file), fPos) == 0;
#endif
    }

private:
    FILE *ensureOpened() {
        if (fFile) {
            return fFile;
        }
        if (!fPath) {
            return nullptr;
        }
        FILE *fp = File::Open(*fPath, File::Mode::Write);
        fFile = fp;
        fPath = std::nullopt;
        return fFile;
    }

private:
    FILE *fFile;
    uint64_t fPos = 0;
    std::optional<std::filesystem::path> fPath;
};

} // namespace stream
} // namespace mcfile
