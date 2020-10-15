#pragma once

#if defined(__APPLE__)
#  include <libkern/OSByteOrder.h>
#elif defined(__linux__) || defined(__CYGWIN__)
#  include <endian.h>
#endif

namespace mcfile {
namespace stream {

struct ReadOption {
    bool fLittleEndian;
};

class InputStreamReader {
public:
    explicit InputStreamReader(std::shared_ptr<InputStream> stream, ReadOption option = { .fLittleEndian = false })
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

    bool seek(int64_t pos) {
        return fStream->seek(pos);
    }

    bool read(uint8_t *v) {
        return fStream->read(v, sizeof(uint8_t), 1);
    }

    bool read(int8_t *v) {
        return fStream->read(v, sizeof(int8_t), 1);
    }

    bool read(int16_t *v) {
        uint16_t t;
        if (!readRaw(&t)) {
            return false;
        }
        t = int16FromRaw(t);
        *v = *(int16_t *) &t;
        return true;
    }

    bool read(uint16_t *v) {
        uint16_t t;
        if (!readRaw(&t)) {
            return false;
        }
        *v = int16FromRaw(t);
        return true;
    }

    bool read(int32_t *v) {
        uint32_t t;
        if (!readRaw(&t)) {
            return false;
        }
        t = int32FromRaw(t);
        *v = *(int32_t *) &t;
        return true;
    }

    bool read(uint32_t *v) {
        uint32_t t;
        if (!readRaw(&t)) {
            return false;
        }
        *v = int32FromRaw(t);
        return true;
    }

    bool read(int64_t *v) {
        uint64_t t;
        if (!readRaw(&t)) {
            return false;
        }
        t = int64FromRaw(t);
        *v = *(int64_t *) &t;
        return true;
    }

    bool read(uint64_t *v) {
        uint64_t t;
        if (!readRaw(&t)) {
            return false;
        }
        *v = int64FromRaw(t);
        return true;
    }

    bool read(std::vector<uint8_t> &buffer) {
        size_t const count = buffer.size();
        return fStream->read(buffer.data(), sizeof(uint8_t), count);
    }

    template<typename T>
    bool copy(std::vector<T>& buffer) {
        return fStream->read(buffer.data(), sizeof(T), buffer.size());
    }

    bool read(std::string &s) {
        uint16_t length;
        if (!read(&length)) {
            return false;
        }
        std::vector<uint8_t> buffer(length);
        if (!read(buffer)) {
            return false;
        }
        buffer.push_back(0);
        std::string tmp((char const *) buffer.data());
        s.swap(tmp);
        return true;
    }

    long length() const {
        if (!fStream) {
            return 0;
        }
        return fStream->length();
    }

    long pos() const {
        if (!fStream) {
            return 0;
        }
        return fStream->pos();
    }

    static uint64_t Int64FromBE(uint64_t v) {
        #if defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)
            return v;
        #else
            return SwapInt64(v);
        #endif
    }

    static uint32_t Int32FromBE(uint32_t v) {
        #if defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)
            return v;
        #else
            return SwapInt32(v);
        #endif
    }

    static uint16_t Int16FromBE(uint16_t v) {
        #if defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)
            return v;
        #else
            return SwapInt16(v);
        #endif
    }

    static uint64_t Int64FromLE(uint64_t v) {
        #if defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)
            return SwapInt64(v);
        #else
            return v;
        #endif
    }

    static uint32_t Int32FromLE(uint32_t v) {
        #if defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)
            return SwapInt32(v);
        #else
            return v;
        #endif
    }

    static uint16_t Int16FromLE(uint16_t v) {
        #if defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)
            return SwapInt16(v);
        #else
            return v;
        #endif
    }

    static uint64_t Int64BEFromNative(uint64_t v) {
        return Int64FromBE(v);
    }

    static uint32_t Int32BEFromNative(uint32_t v) {
        return Int32FromBE(v);
    }
    
    static uint16_t Int16BEFromNative(uint16_t v) {
        return Int16FromBE(v);
    }
    
private:
    template<typename T>
    bool readRaw(T *v) {
        return fStream->read(v, sizeof(T), 1);
    }

    uint64_t int64FromRaw(uint64_t v) const {
        if (fLittleEndian) {
            return Int64FromLE(v);
        } else {
            return Int64FromBE(v);
        }
    }

    uint32_t int32FromRaw(uint32_t v) const {
        if (fLittleEndian) {
            return Int32FromLE(v);
        } else {
            return Int32FromBE(v);
        }
    }

    uint16_t int16FromRaw(uint16_t v) const {
        if (fLittleEndian) {
            return Int16FromLE(v);
        } else {
            return Int16FromBE(v);
        }
    }

    static uint64_t SwapInt64(uint64_t v) {
        return ((v & 0x00000000000000ffLL) << 56)
               | ((v & 0x000000000000ff00LL) << 40)
               | ((v & 0x0000000000ff0000LL) << 24)
               | ((v & 0x00000000ff000000LL) << 8)
               | ((v & 0x000000ff00000000LL) >> 8)
               | ((v & 0x0000ff0000000000LL) >> 24)
               | ((v & 0x00ff000000000000LL) >> 40)
               | ((v & 0xff00000000000000LL) >> 56);
    }

    static uint32_t SwapInt32(uint32_t v) {
        uint32_t r;
        r = v << 24;
        r |= (v & 0x0000FF00) << 8;
        r |= (v & 0x00FF0000) >> 8;
        r |= v >> 24;
        return r;
    }

    static uint16_t SwapInt16(uint16_t v) {
        uint16_t r;
        r = v << 8;
        r |= (v & 0xFF00) >> 8;
        return r;
    }

private:
    std::shared_ptr<InputStream> fStream;
    bool fLittleEndian;
};

} // namespace stream
} // namespace mcfile
