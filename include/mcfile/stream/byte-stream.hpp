#pragma once

namespace mcfile::stream {

class ByteStream : public InputStream, public OutputStream {
public:
    explicit ByteStream(std::vector<uint8_t> &buffer)
        : fLoc(0) {
        fBuffer.swap(buffer);
    }

    explicit ByteStream(std::string const &s)
        : fLoc(0) {
        fBuffer.reserve(s.size());
        std::copy(s.begin(), s.end(), std::back_inserter(fBuffer));
    }

    ByteStream()
        : fLoc(0) {}

    ~ByteStream() override {}

    ByteStream(ByteStream const &) = delete;

    ByteStream &operator=(ByteStream const &) = delete;

    size_t read(void *buf, size_t size) override {
        if (fLoc >= fBuffer.size()) {
            return 0;
        }
        if (size == 0) {
            return 0;
        }
        uint64_t next = (std::min)(fLoc + size, (uint64_t)fBuffer.size());
        size_t read = next - fLoc;
        std::copy(fBuffer.begin() + fLoc, fBuffer.begin() + next, (uint8_t *)buf);
        fLoc = next;
        return read;
    }

    bool write(void const *buf, size_t size) override {
        if (fBuffer.size() <= fLoc + size) {
            size_t add = fLoc + size - fBuffer.size();
            try {
                for (size_t i = 0; i < add; i++) {
                    fBuffer.push_back(0);
                }
            } catch (...) {
                return false;
            }
        }
        for (size_t i = 0; i < size; i++) {
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

    bool valid() const override {
        return true;
    }

    uint64_t pos() const override { return fLoc; }

    bool truncate() override {
        if (fBuffer.size() > fLoc) {
            fBuffer.resize(fLoc);
        }
        return true;
    }

    void drain(std::vector<uint8_t> &out) {
        out.clear();
        out.swap(fBuffer);
        fLoc = 0;
    }

    void drain(std::string &out) {
        out.clear();
        std::copy(fBuffer.begin(), fBuffer.end(), std::back_inserter(out));
        std::vector<uint8_t>().swap(fBuffer);
        fLoc = 0;
    }

private:
    std::vector<uint8_t> fBuffer;
    uint64_t fLoc;
};

} // namespace mcfile::stream
