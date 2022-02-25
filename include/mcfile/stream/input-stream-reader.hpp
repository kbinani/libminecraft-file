#pragma once

namespace mcfile::stream {

class InputStreamReader {
public:
    explicit InputStreamReader(std::shared_ptr<InputStream> stream, std::endian endian = std::endian::big)
        : fStream(stream)
        , fEndian(endian) {
    }

    InputStreamReader(InputStreamReader const &) = delete;

    InputStreamReader &operator=(InputStreamReader const &) = delete;

    bool valid() const {
        if (!fStream) {
            return false;
        }
        return fStream->valid();
    }

    [[nodiscard]] bool seek(uint64_t pos) {
        return fStream->seek(pos);
    }

    [[nodiscard]] bool read(uint8_t *v) {
        return fStream->read(v, sizeof(uint8_t)) == sizeof(uint8_t);
    }

    [[nodiscard]] bool read(int8_t *v) {
        return fStream->read(v, sizeof(int8_t)) == sizeof(int8_t);
    }

    [[nodiscard]] bool read(int16_t *v) {
        if (!readRaw(v)) {
            return false;
        }
        if (fEndian != std::endian::native) {
            *v = SwapI16(*v);
        }
        return true;
    }

    [[nodiscard]] bool read(uint16_t *v) {
        if (!readRaw(v)) {
            return false;
        }
        if (fEndian != std::endian::native) {
            *v = SwapU16(*v);
        }
        return true;
    }

    [[nodiscard]] bool read(int32_t *v) {
        if (!readRaw(v)) {
            return false;
        }
        if (fEndian != std::endian::native) {
            *v = SwapI32(*v);
        }
        return true;
    }

    [[nodiscard]] bool read(uint32_t *v) {
        if (!readRaw(v)) {
            return false;
        }
        if (fEndian != std::endian::native) {
            *v = SwapU32(*v);
        }
        return true;
    }

    [[nodiscard]] bool read(int64_t *v) {
        if (!readRaw(v)) {
            return false;
        }
        if (fEndian != std::endian::native) {
            *v = SwapI64(*v);
        }
        return true;
    }

    [[nodiscard]] bool read(uint64_t *v) {
        if (!readRaw(v)) {
            return false;
        }
        if (fEndian != std::endian::native) {
            *v = SwapU64(*v);
        }
        return true;
    }

    template<class T>
    [[nodiscard]] bool read(std::vector<T> &buffer) {
        static_assert(std::is_standard_layout_v<T>);
        if (fStream->read(buffer.data(), buffer.size() * sizeof(T)) != buffer.size() * sizeof(T)) {
            return false;
        }
        if (fEndian != std::endian::native) {
            size_t size = buffer.size();
            for (size_t i = 0; i < size; i++) {
                buffer[i] = ByteSwap(buffer[i]);
            }
        }
        return true;
    }

    [[nodiscard]] bool read(std::string &s) {
        uint16_t length;
        if (!read(&length)) {
            return false;
        }
        std::vector<uint8_t> buffer(length);
        if (!read(buffer)) {
            return false;
        }
        buffer.push_back(0);
        std::string tmp((char const *)buffer.data());
        s.swap(tmp);
        return true;
    }

    uint64_t pos() const {
        if (!fStream) {
            return 0;
        }
        return fStream->pos();
    }

private:
    template<class T>
    [[nodiscard]] bool readRaw(T *v) {
        return fStream->read(v, sizeof(T)) == sizeof(T);
    }

private:
    std::shared_ptr<InputStream> fStream;
    std::endian const fEndian;
};

} // namespace mcfile::stream
