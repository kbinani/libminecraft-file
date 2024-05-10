#pragma once

namespace mcfile::stream {

class InputStreamReader {
public:
    explicit InputStreamReader(std::shared_ptr<InputStream> stream, Encoding encoding = Encoding::Java)
        : fEndian(encoding == Encoding::Java ? Endian::Big : Endian::Little)
        , fEncoding(encoding)
        , fStream(stream) {
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
        if (fEndian != Endian::Native) {
            *v = SwapI16(*v);
        }
        return true;
    }

    [[nodiscard]] bool read(uint16_t *v) {
        if (!readRaw(v)) {
            return false;
        }
        if (fEndian != Endian::Native) {
            *v = SwapU16(*v);
        }
        return true;
    }

    [[nodiscard]] bool read(int32_t *v) {
        if (!readRaw(v)) {
            return false;
        }
        if (fEndian != Endian::Native) {
            *v = SwapI32(*v);
        }
        return true;
    }

    [[nodiscard]] bool read(uint32_t *v) {
        if (!readRaw(v)) {
            return false;
        }
        if (fEndian != Endian::Native) {
            *v = SwapU32(*v);
        }
        return true;
    }

    [[nodiscard]] bool read(int64_t *v) {
        if (!readRaw(v)) {
            return false;
        }
        if (fEndian != Endian::Native) {
            *v = SwapI64(*v);
        }
        return true;
    }

    [[nodiscard]] bool read(uint64_t *v) {
        if (!readRaw(v)) {
            return false;
        }
        if (fEndian != Endian::Native) {
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
        if (fEndian != Endian::Native) {
            size_t size = buffer.size();
            for (size_t i = 0; i < size; i++) {
                buffer[i] = ByteSwap(buffer[i]);
            }
        }
        return true;
    }

    [[nodiscard]] bool read(std::u8string &s) {
        uint16_t length;
        if (!read(&length)) {
            return false;
        }
        std::string buffer;
        buffer.resize(length);
        if (fStream->read(buffer.data(), length) != length) {
            return false;
        }
        if (fEncoding == Encoding::Java) {
            if (auto r = String::Utf8FromJavaUtf8(buffer); r) {
                s.swap(*r);
                return true;
            } else {
                return false;
            }
        } else {
            s.clear();
            std::copy(buffer.begin(), buffer.end(), std::back_inserter(s));
            return true;
        }
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

public:
    Endian const fEndian;
    Encoding const fEncoding;

private:
    std::shared_ptr<InputStream> fStream;
};

} // namespace mcfile::stream
