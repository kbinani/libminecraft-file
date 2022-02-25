#pragma once

namespace mcfile {

constexpr bool IsBigEndian() {
    return std::endian::native == std::endian::big;
}

inline uint64_t SwapU64(uint64_t v) {
#if defined(_MSC_VER)
    return _byteswap_uint64(v);
#else
    return ((v & 0x00000000000000ffLL) << 56) | ((v & 0x000000000000ff00LL) << 40) | ((v & 0x0000000000ff0000LL) << 24) | ((v & 0x00000000ff000000LL) << 8) | ((v & 0x000000ff00000000LL) >> 8) | ((v & 0x0000ff0000000000LL) >> 24) | ((v & 0x00ff000000000000LL) >> 40) | ((v & 0xff00000000000000LL) >> 56);
#endif
}

inline int64_t SwapI64(int64_t v) {
    uint64_t u = *(uint64_t *)&v;
    u = SwapU64(u);
    return *(int64_t *)&u;
}

inline uint32_t SwapU32(uint32_t v) {
#if defined(_MSC_VER)
    static_assert(sizeof(unsigned long) == sizeof(uint32_t));
    return _byteswap_ulong(v);
#else
    uint32_t r;
    r = v << 24;
    r |= (v & 0x0000FF00) << 8;
    r |= (v & 0x00FF0000) >> 8;
    r |= v >> 24;
    return r;
#endif
}

inline int32_t SwapI32(int32_t v) {
    uint32_t u = *(uint32_t *)&v;
    u = SwapU32(u);
    return *(int32_t *)&u;
}

inline uint16_t SwapU16(uint16_t v) {
#if defined(_MSC_VER)
    static_assert(sizeof(unsigned short) == sizeof(uint16_t));
    return _byteswap_ushort(v);
#else
    uint16_t r;
    r = v << 8;
    r |= (v & 0xFF00) >> 8;
    return r;
#endif
}

inline int16_t SwapI16(int16_t v) {
    uint16_t u = *(uint16_t *)&v;
    u = SwapU16(u);
    return *(int16_t *)&u;
}

template<class T>
inline T ByteSwap(T v) {
    static_assert(std::is_same_v<T, uint64_t> || std::is_same_v<T, int64_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, int32_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>);
    if constexpr (std::is_same_v<T, uint64_t>) {
        return SwapU64(v);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return SwapI64(v);
    } else if constexpr (std::is_same_v<T, uint32_t>) {
        return SwapU32(v);
    } else if constexpr (std::is_same_v<T, int32_t>) {
        return SwapI32(v);
    } else if constexpr (std::is_same_v<T, uint16_t>) {
        return SwapU16(v);
    } else if constexpr (std::is_same_v<T, int16_t>) {
        return SwapI16(v);
    } else {
        return v;
    }
}

inline uint64_t U64FromBE(uint64_t v) {
    if constexpr (IsBigEndian()) {
        return v;
    } else {
        return SwapU64(v);
    }
}

inline uint32_t U32FromBE(uint32_t v) {
    if constexpr (IsBigEndian()) {
        return v;
    } else {
        return SwapU32(v);
    }
}

inline uint16_t U16FromBE(uint16_t v) {
    if constexpr (IsBigEndian()) {
        return v;
    } else {
        return SwapU16(v);
    }
}

inline int64_t I64FromBE(int64_t v) {
    if constexpr (IsBigEndian()) {
        return v;
    } else {
        return SwapI64(v);
    }
}

inline int32_t I32FromBE(int32_t v) {
    if constexpr (IsBigEndian()) {
        return v;
    } else {
        return SwapI32(v);
    }
}

inline int16_t I16FromBE(int16_t v) {
    if constexpr (IsBigEndian()) {
        return v;
    } else {
        return SwapI16(v);
    }
}

inline uint64_t U64FromLE(uint64_t v) {
    if constexpr (IsBigEndian()) {
        return SwapU64(v);
    } else {
        return v;
    }
}

inline uint32_t U32FromLE(uint32_t v) {
    if constexpr (IsBigEndian()) {
        return SwapU32(v);
    } else {
        return v;
    }
}

inline uint16_t U16FromLE(uint16_t v) {
    if constexpr (IsBigEndian()) {
        return SwapU16(v);
    } else {
        return v;
    }
}

inline int64_t I64FromLE(int64_t v) {
    if constexpr (IsBigEndian()) {
        return SwapI64(v);
    } else {
        return v;
    }
}

inline int32_t I32FromLE(int32_t v) {
    if constexpr (IsBigEndian()) {
        return SwapI32(v);
    } else {
        return v;
    }
}

inline int16_t I16FromLE(int16_t v) {
    if constexpr (IsBigEndian()) {
        return SwapI16(v);
    } else {
        return v;
    }
}

inline uint64_t U64BEFromNative(uint64_t v) {
    if constexpr (IsBigEndian()) {
        return v;
    } else {
        return SwapU64(v);
    }
}

inline uint32_t U32BEFromNative(uint32_t v) {
    if constexpr (IsBigEndian()) {
        return v;
    } else {
        return SwapU32(v);
    }
}

inline uint16_t U16BEFromNative(uint16_t v) {
    if constexpr (IsBigEndian()) {
        return v;
    } else {
        return SwapU16(v);
    }
}

inline int64_t I64BEFromNative(int64_t v) {
    if constexpr (IsBigEndian()) {
        return v;
    } else {
        return SwapI64(v);
    }
}

inline int32_t I32BEFromNative(int32_t v) {
    if constexpr (IsBigEndian()) {
        return v;
    } else {
        return SwapI32(v);
    }
}

inline int16_t I16BEFromNative(int16_t v) {
    if constexpr (IsBigEndian()) {
        return v;
    } else {
        return SwapI16(v);
    }
}

inline uint64_t U64LEFromNative(uint64_t v) {
    if constexpr (IsBigEndian()) {
        return SwapU64(v);
    } else {
        return v;
    }
}

inline uint32_t U32LEFromNative(uint32_t v) {
    if constexpr (IsBigEndian()) {
        return SwapU32(v);
    } else {
        return v;
    }
}

inline uint16_t U16LEFromNative(uint16_t v) {
    if constexpr (IsBigEndian()) {
        return SwapU16(v);
    } else {
        return v;
    }
}

inline int64_t I64LEFromNative(int64_t v) {
    if constexpr (IsBigEndian()) {
        return SwapI64(v);
    } else {
        return v;
    }
}

inline int32_t I32LEFromNative(int32_t v) {
    if constexpr (IsBigEndian()) {
        return SwapI32(v);
    } else {
        return v;
    }
}

inline int16_t I16LEFromNative(int16_t v) {
    if constexpr (IsBigEndian()) {
        return SwapI16(v);
    } else {
        return v;
    }
}

} // namespace mcfile
