#pragma once

namespace mcfile {
namespace stream {

struct WriteOption {
    bool fLittleEndian;
};

class OutputStreamWriter {
public:
    explicit OutputStreamWriter(std::shared_ptr<OutputStream> stream, WriteOption option = {.fLittleEndian = false})
        : fStream(stream)
        , fLittleEndian(option.fLittleEndian) {
    }

    OutputStreamWriter(OutputStreamWriter const &) = delete;

    OutputStreamWriter &operator=(OutputStreamWriter const &) = delete;

    void write(uint8_t v) {
        fStream->write(&v, sizeof(v));
    }

    void write(uint64_t v) {
        if (fLittleEndian) {
            v = mcfile::Int64LEFromNative(v);
        } else {
            v = mcfile::Int64BEFromNative(v);
        }
        fStream->write(&v, sizeof(v));
    }

    void write(uint32_t v) {
        if (fLittleEndian) {
            v = mcfile::Int32LEFromNative(v);
        } else {
            v = mcfile::Int32BEFromNative(v);
        }
        fStream->write(&v, sizeof(v));
    }

    void write(uint16_t v) {
        if (fLittleEndian) {
            v = mcfile::Int16LEFromNative(v);
        } else {
            v = mcfile::Int16BEFromNative(v);
        }
        fStream->write(&v, sizeof(v));
    }

    void write(int64_t v) {
        uint64_t t;
        if (fLittleEndian) {
            t = mcfile::Int64LEFromNative(*(int64_t *)&v);
        } else {
            t = mcfile::Int64BEFromNative(*(int64_t *)&v);
        }
        fStream->write(&t, sizeof(t));
    }

    void write(int32_t v) {
        uint32_t t;
        if (fLittleEndian) {
            t = mcfile::Int32LEFromNative(*(int32_t *)&v);
        } else {
            t = mcfile::Int32BEFromNative(*(int32_t *)&v);
        }
        fStream->write(&t, sizeof(t));
    }

    void write(int16_t v) {
        uint16_t t;
        if (fLittleEndian) {
            t = mcfile::Int16LEFromNative(*(int16_t *)&v);
        } else {
            t = mcfile::Int16BEFromNative(*(int16_t *)&v);
        }
        fStream->write(&t, sizeof(t));
    }

    void write(std::vector<uint8_t> &buffer) {
        fStream->write(buffer.data(), buffer.size());
    }

    void write(std::string const &s) {
        uint16_t length = (uint16_t)std::min(s.size(), (size_t)std::numeric_limits<uint16_t>::max());
        write(length);
        fStream->write((void *)s.data(), length);
    }

private:
    uint64_t int64LE(uint64_t v) const {
        if (fLittleEndian) {
            return mcfile::Int64LEFromNative(v);
        } else {
            return mcfile::Int64LEFromNative(v);
        }
    }

    uint32_t int32FromRaw(uint32_t v) const {
        if (fLittleEndian) {
            return mcfile::Int32LEFromNative(v);
        } else {
            return mcfile::Int32FromBE(v);
        }
    }

    uint16_t int16FromRaw(uint16_t v) const {
        if (fLittleEndian) {
            return mcfile::Int16FromLE(v);
        } else {
            return mcfile::Int16FromBE(v);
        }
    }

private:
    std::shared_ptr<OutputStream> fStream;
    bool fLittleEndian;
};

} // namespace stream
} // namespace mcfile
