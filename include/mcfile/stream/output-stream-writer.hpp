#pragma once

namespace mcfile {
namespace stream {

struct WriteOption {
    bool fLittleEndian;
};

class OutputStreamWriter {
public:
    explicit OutputStreamWriter(std::shared_ptr<OutputStream> stream, WriteOption option = { .fLittleEndian = false })
        : fStream(stream)
        , fLittleEndian(option.fLittleEndian) {
    }

    OutputStreamWriter(OutputStreamWriter const &) = delete;

    OutputStreamWriter &operator=(OutputStreamWriter const &) = delete;

    void write(uint8_t v) {
        fStream->write(&v, sizeof(v));
    }
    
    void write(uint64_t v) {
        v = detail::Int64LEFromNative(v);
        fStream->write(&v, sizeof(v));
    }

    void write(uint32_t v) {
        v = detail::Int32LEFromNative(v);
        fStream->write(&v, sizeof(v));
    }

    void write(uint16_t v) {
        v = detail::Int16LEFromNative(v);
        fStream->write(&v, sizeof(v));
    }

    void write(int64_t v) {
        uint64_t t = detail::Int64LEFromNative(*(int64_t *) &v);
        fStream->write(&t, sizeof(t));
    }

    void write(int32_t v) {
        uint64_t t = detail::Int32LEFromNative(*(int32_t *) &v);
        fStream->write(&t, sizeof(t));
    }

    void write(int16_t v) {
        uint16_t t = detail::Int16LEFromNative(*(int16_t *) &v);
        fStream->write(&t, sizeof(t));
    }

    void write(std::vector<uint8_t> &buffer) {
        fStream->write(buffer.data(), buffer.size());
    }

    void write(std::string const& s) {
        uint16_t length = (uint16_t)std::min(s.size(), (size_t)std::numeric_limits<uint16_t>::max());
        write(length);
        fStream->write((void *)s.data(), length);
    }
    
private:
    uint64_t int64LE(uint64_t v) const {
        if (fLittleEndian) {
            return detail::Int64LEFromNative(v);
        } else {
            return detail::Int64LEFromNative(v);
        }
    }

    uint32_t int32FromRaw(uint32_t v) const {
        if (fLittleEndian) {
            return detail::Int32LEFromNative(v);
        } else {
            return detail::Int32FromBE(v);
        }
    }

    uint16_t int16FromRaw(uint16_t v) const {
        if (fLittleEndian) {
            return detail::Int16FromLE(v);
        } else {
            return detail::Int16FromBE(v);
        }
    }

private:
    std::shared_ptr<OutputStream> fStream;
    bool fLittleEndian;
};

}
}
