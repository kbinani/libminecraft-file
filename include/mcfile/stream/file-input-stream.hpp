#pragma once

namespace mcfile {
namespace stream {

class FileInputStream : public InputStream {
public:
    explicit FileInputStream(std::string const &filePath)
            : fFile(nullptr), fLoc(0) {
        FILE *fp = fopen(filePath.c_str(), "rb");

        if (!fp) {
            return;
        }
        if (fseek(fp, 0, SEEK_END) != 0) {
            fclose(fp);
            return;
        }
        long length = ftell(fp);
        if (length == -1L) {
            fclose(fp);
            return;
        }
        if (fseek(fp, 0, SEEK_SET) != 0) {
            fclose(fp);
            return;
        }
        this->fFile = fp;
        this->fLength = length;
    }

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
        fLoc += size * count;
        return fread(buffer, size, count, fFile) == count;
    }

    bool seek(long offset) override {
        if (!fFile) {
            return false;
        }
        fLoc = offset;
        return fseek(fFile, offset, SEEK_SET) == 0;
    }

    bool valid() const override {
        return fFile != nullptr;
    }

    long length() const override { return fLength; }

    long pos() const override { return fLoc; }

    static bool Copy(FILE *in, FILE *out, size_t length) {
        if (!in || !out) {
            return false;
        }
        if (length == 0) {
            return true;
        }
        uint8_t buffer[512]= { 0 };
        size_t const size = sizeof(buffer[0]);
        size_t const nitems = sizeof(buffer) / size;
        size_t remain = length;
        while (remain > 0) {
            size_t const num = std::min(nitems, remain);
            size_t const read = fread(buffer, size, num, in);
            if (read != num) {
                return false;
            }
            size_t const written = fwrite(buffer, size, read, out);
            if (written != read) {
                return false;
            }
            assert(remain >= read);
            remain -= read;
        }
        return true;
    }
    
private:
    FILE *fFile;
    long fLength;
    long fLoc;
};

} // namespace stream
} // namespace mcfile
