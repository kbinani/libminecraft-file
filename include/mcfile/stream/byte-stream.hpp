#pragma once

namespace mcfile {
namespace stream {

class ByteStream : public InputStream {
public:
    explicit ByteStream(std::vector<uint8_t> &buffer)
            : fLoc(0) {
        this->fBuffer.swap(buffer);
    }

    ~ByteStream() {}

    ByteStream(ByteStream const &) = delete;

    ByteStream &operator=(ByteStream const &) = delete;

    bool read(void *buf, size_t size, size_t count) override {
        if (fBuffer.size() <= fLoc + size * count) {
            return false;
        }
        std::copy(fBuffer.begin() + fLoc, fBuffer.begin() + fLoc + size * count, (uint8_t *) buf);
        fLoc += size * count;
        return true;
    }

    bool seek(long offset) override {
        if (offset < 0 || fBuffer.size() <= offset) {
            return false;
        }
        fLoc = offset;
        return true;
    }

    long length() const override { return fBuffer.size(); }

    bool valid() const override { return true; }

    long pos() const override { return fLoc; }

private:
    std::vector<uint8_t> fBuffer;
    long fLoc;
};

} // namespace stream
} // namespace mcfile
