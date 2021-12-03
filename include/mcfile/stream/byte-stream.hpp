#pragma once

namespace mcfile {
namespace stream {

class ByteStream : public InputStream, public OutputStream {
public:
    explicit ByteStream(std::vector<uint8_t> &buffer)
        : fLoc(0) {
        this->fBuffer.swap(buffer);
    }

    ByteStream()
        : fLoc(0) {}

    ~ByteStream() {}

    ByteStream(ByteStream const &) = delete;

    ByteStream &operator=(ByteStream const &) = delete;

    bool read(void *buf, size_t size, size_t count) override {
        if (fBuffer.size() < fLoc + size * count) {
            return false;
        }
        std::copy(fBuffer.begin() + fLoc, fBuffer.begin() + fLoc + size * count, (uint8_t *)buf);
        fLoc += size * count;
        return true;
    }

    bool write(void const *buf, size_t size) override {
        if (fBuffer.size() <= fLoc + size) {
            size_t add = fLoc + size - fBuffer.size();
            try {
                for (int i = 0; i < add; i++) {
                    fBuffer.push_back(0);
                }
            } catch (...) {
                return false;
            }
        }
        for (int i = 0; i < size; i++) {
            fBuffer[fLoc + i] = ((uint8_t *)buf)[i];
        }
        fLoc += size;
        return true;
    }

    bool seek(uint64_t offset) override {
        if (offset < 0 || fBuffer.size() <= offset) {
            return false;
        }
        fLoc = offset;
        return true;
    }

    uint64_t length() const override { return fBuffer.size(); }

    bool valid() const override { return true; }

    uint64_t pos() const override { return fLoc; }

    void drain(std::vector<uint8_t> &out) {
        out.clear();
        out.swap(fBuffer);
        fLoc = 0;
    }

private:
    std::vector<uint8_t> fBuffer;
    uint64_t fLoc;
};

} // namespace stream
} // namespace mcfile
