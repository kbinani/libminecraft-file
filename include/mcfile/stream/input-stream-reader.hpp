#pragma once

#if defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#elif defined(__linux__) || defined(__CYGWIN__)
#include <endian.h>
#endif

namespace mcfile::stream {

struct ReadOption {
    bool fLittleEndian;
};

class InputStreamReader {
public:
    explicit InputStreamReader(std::shared_ptr<InputStream> stream, ReadOption option = {.fLittleEndian = false})
        : fStream(stream)
        , fLittleEndian(option.fLittleEndian) {
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
        uint16_t t;
        if (!readRaw(&t)) {
            return false;
        }
        t = int16FromRaw(t);
        *v = *(int16_t *)&t;
        return true;
    }

    [[nodiscard]] bool read(uint16_t *v) {
        uint16_t t;
        if (!readRaw(&t)) {
            return false;
        }
        *v = int16FromRaw(t);
        return true;
    }

    [[nodiscard]] bool read(int32_t *v) {
        uint32_t t;
        if (!readRaw(&t)) {
            return false;
        }
        t = int32FromRaw(t);
        *v = *(int32_t *)&t;
        return true;
    }

    [[nodiscard]] bool read(uint32_t *v) {
        uint32_t t;
        if (!readRaw(&t)) {
            return false;
        }
        *v = int32FromRaw(t);
        return true;
    }

    [[nodiscard]] bool read(int64_t *v) {
        uint64_t t;
        if (!readRaw(&t)) {
            return false;
        }
        t = int64FromRaw(t);
        *v = *(int64_t *)&t;
        return true;
    }

    [[nodiscard]] bool read(uint64_t *v) {
        uint64_t t;
        if (!readRaw(&t)) {
            return false;
        }
        *v = int64FromRaw(t);
        return true;
    }

    template<typename T>
    [[nodiscard]] bool read(std::vector<T> &buffer) {
        static_assert(std::is_standard_layout_v<T>);
        return fStream->read(buffer.data(), sizeof(T) * buffer.size()) == sizeof(T) * buffer.size();
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

    bool isLittleEndian() const {
        return fLittleEndian;
    }

private:
    template<typename T>
    [[nodiscard]] bool readRaw(T *v) {
        return fStream->read(v, sizeof(T)) == sizeof(T);
    }

    uint64_t int64FromRaw(uint64_t v) const {
        if (fLittleEndian) {
            return mcfile::Int64FromLE(v);
        } else {
            return mcfile::Int64FromBE(v);
        }
    }

    uint32_t int32FromRaw(uint32_t v) const {
        if (fLittleEndian) {
            return mcfile::Int32FromLE(v);
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
    std::shared_ptr<InputStream> fStream;
    bool fLittleEndian;
};

} // namespace mcfile::stream
