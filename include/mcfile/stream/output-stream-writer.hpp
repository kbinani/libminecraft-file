#pragma once

namespace mcfile::stream {

class OutputStreamWriter {
public:
    explicit OutputStreamWriter(std::shared_ptr<OutputStream> stream, Encoding encoding = Encoding::Java)
        : fStream(stream)
        , fEndian(encoding == Encoding::Java ? Endian::Big : Endian::Little)
        , fEncoding(encoding) {
    }

    OutputStreamWriter(OutputStreamWriter const &) = delete;

    OutputStreamWriter &operator=(OutputStreamWriter const &) = delete;

    [[nodiscard]] bool write(uint8_t v) {
        return fStream->write(&v, sizeof(v));
    }

    [[nodiscard]] bool write(uint64_t v) {
        if (fEndian != Endian::Native) {
            v = ByteSwap(v);
        }
        return fStream->write(&v, sizeof(v));
    }

    [[nodiscard]] bool write(uint32_t v) {
        if (fEndian != Endian::Native) {
            v = ByteSwap(v);
        }
        return fStream->write(&v, sizeof(v));
    }

    [[nodiscard]] bool write(uint16_t v) {
        if (fEndian != Endian::Native) {
            v = ByteSwap(v);
        }
        return fStream->write(&v, sizeof(v));
    }

    [[nodiscard]] bool write(int64_t v) {
        if (fEndian != Endian::Native) {
            v = ByteSwap(v);
        }
        return fStream->write(&v, sizeof(v));
    }

    [[nodiscard]] bool write(int32_t v) {
        if (fEndian != Endian::Native) {
            v = ByteSwap(v);
        }
        return fStream->write(&v, sizeof(v));
    }

    [[nodiscard]] bool write(int16_t v) {
        if (fEndian != Endian::Native) {
            v = ByteSwap(v);
        }
        return fStream->write(&v, sizeof(v));
    }

    [[nodiscard]] bool write(std::vector<uint8_t> const &buffer) {
        return fStream->write(buffer.data(), buffer.size());
    }

    [[nodiscard]] bool write(void const *data, size_t size) {
        return fStream->write(data, size);
    }

    [[nodiscard]] bool write(std::u8string const &s) {
        if (fEncoding == Encoding::Java) {
            auto v = String::JavaUtf8FromUtf8(s);
            if (!v) {
                return false;
            }
            if (v->size() > (size_t)std::numeric_limits<uint16_t>::max()) {
                return false;
            }
            uint16_t length = (uint16_t)v->size();
            if (!write(length)) {
                return false;
            }
            return fStream->write((void *)v->data(), length);
        } else {
            if (s.size() > (size_t)std::numeric_limits<uint16_t>::max()) {
                return false;
            }
            uint16_t length = (uint16_t)s.size();
            if (!write(length)) {
                return false;
            }
            return fStream->write((void *)s.data(), length);
        }
    }

private:
    std::shared_ptr<OutputStream> fStream;
    Endian const fEndian;
    Encoding const fEncoding;
};

} // namespace mcfile::stream
