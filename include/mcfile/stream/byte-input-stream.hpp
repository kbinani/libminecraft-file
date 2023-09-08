#pragma once

namespace mcfile::stream {

class ByteInputStream : public InputStream {
public:
    explicit ByteInputStream(std::string_view data)
        : fData(data) {}

    ByteInputStream(char const *data, size_t size)
        : fData(data, size) {}

    size_t read(void *buf, size_t size) override {
        if (fLoc >= fData.size()) {
            return 0;
        }
        if (size == 0) {
            return 0;
        }
        uint64_t next = (std::min)(fLoc + size, (uint64_t)fData.size());
        size_t read = next - fLoc;
        std::copy(fData.begin() + fLoc, fData.begin() + next, (char *)buf);
        fLoc = next;
        return read;
    }

    bool seek(uint64_t offset) override {
        if (fData.size() <= offset) {
            return false;
        }
        fLoc = offset;
        return true;
    }

    bool valid() const override {
        return true;
    }

    uint64_t pos() const override { return fLoc; }

private:
    std::string_view const fData;
    uint64_t fLoc = 0;
};

} // namespace mcfile::stream
