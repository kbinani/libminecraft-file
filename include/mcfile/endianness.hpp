#pragma once

namespace mcfile {

inline uint64_t SwapInt64(uint64_t v) {
    return ((v & 0x00000000000000ffLL) << 56)
           | ((v & 0x000000000000ff00LL) << 40)
           | ((v & 0x0000000000ff0000LL) << 24)
           | ((v & 0x00000000ff000000LL) << 8)
           | ((v & 0x000000ff00000000LL) >> 8)
           | ((v & 0x0000ff0000000000LL) >> 24)
           | ((v & 0x00ff000000000000LL) >> 40)
           | ((v & 0xff00000000000000LL) >> 56);
}

inline uint32_t SwapInt32(uint32_t v) {
    uint32_t r;
    r = v << 24;
    r |= (v & 0x0000FF00) << 8;
    r |= (v & 0x00FF0000) >> 8;
    r |= v >> 24;
    return r;
}

inline uint16_t SwapInt16(uint16_t v) {
    uint16_t r;
    r = v << 8;
    r |= (v & 0xFF00) >> 8;
    return r;
}

inline uint64_t Int64FromBE(uint64_t v) {
    #if defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)
        return v;
    #else
        return SwapInt64(v);
    #endif
}

inline uint32_t Int32FromBE(uint32_t v) {
    #if defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)
        return v;
    #else
        return SwapInt32(v);
    #endif
}

inline uint16_t Int16FromBE(uint16_t v) {
    #if defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)
        return v;
    #else
        return SwapInt16(v);
    #endif
}

inline uint64_t Int64FromLE(uint64_t v) {
    #if defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)
        return SwapInt64(v);
    #else
        return v;
    #endif
}

inline uint32_t Int32FromLE(uint32_t v) {
    #if defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)
        return SwapInt32(v);
    #else
        return v;
    #endif
}

inline uint16_t Int16FromLE(uint16_t v) {
    #if defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN)
        return SwapInt16(v);
    #else
        return v;
    #endif
}

inline uint64_t Int64BEFromNative(uint64_t v) {
    return Int64FromBE(v);
}

inline uint32_t Int32BEFromNative(uint32_t v) {
    return Int32FromBE(v);
}

inline uint16_t Int16BEFromNative(uint16_t v) {
    return Int16FromBE(v);
}

inline uint64_t Int64LEFromNative(uint64_t v) {
    return Int64FromLE(v);
}

inline uint32_t Int32LEFromNative(uint32_t v) {
    return Int32FromLE(v);
}

inline uint16_t Int16LEFromNative(uint16_t v) {
    return Int16FromLE(v);
}

}
