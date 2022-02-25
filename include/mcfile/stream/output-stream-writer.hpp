#pragma once

namespace mcfile::stream {

class OutputStreamWriter {
public:
    explicit OutputStreamWriter(std::shared_ptr<OutputStream> stream, std::endian endian = std::endian::big)
        : fStream(stream)
        , fEndian(endian) {
    }

    OutputStreamWriter(OutputStreamWriter const &) = delete;

    OutputStreamWriter &operator=(OutputStreamWriter const &) = delete;

    [[nodiscard]] bool write(uint8_t v) {
        return fStream->write(&v, sizeof(v));
    }

    [[nodiscard]] bool write(uint64_t v) {
        if (fEndian != std::endian::native) {
            v = ByteSwap(v);
        }
        return fStream->write(&v, sizeof(v));
    }

    [[nodiscard]] bool write(uint32_t v) {
        if (fEndian != std::endian::native) {
            v = ByteSwap(v);
        }
        return fStream->write(&v, sizeof(v));
    }

    [[nodiscard]] bool write(uint16_t v) {
        if (fEndian != std::endian::native) {
            v = ByteSwap(v);
        }
        return fStream->write(&v, sizeof(v));
    }

    [[nodiscard]] bool write(int64_t v) {
        if (fEndian != std::endian::native) {
            v = ByteSwap(v);
        }
        return fStream->write(&v, sizeof(v));
    }

    [[nodiscard]] bool write(int32_t v) {
        if (fEndian != std::endian::native) {
            v = ByteSwap(v);
        }
        return fStream->write(&v, sizeof(v));
    }

    [[nodiscard]] bool write(int16_t v) {
        if (fEndian != std::endian::native) {
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

    [[nodiscard]] bool write(std::string const &s) {
        uint16_t length = (uint16_t)std::min(s.size(), (size_t)std::numeric_limits<uint16_t>::max());
        if (!write(length)) {
            return false;
        }
        return fStream->write((void *)s.data(), length);
    }

private:
    std::shared_ptr<OutputStream> fStream;
    std::endian const fEndian;
};

} // namespace mcfile::stream
