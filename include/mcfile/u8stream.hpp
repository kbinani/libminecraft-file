#pragma once

namespace mcfile::u8stream {

inline std::ostream &operator<<(std::ostream &os, std::u8string_view const &s) {
#if defined(_MSC_VER)
    int u16Length = ::MultiByteToWideChar(CP_UTF8, 0, (char const *)s.data(), s.size(), nullptr, 0);
    if (u16Length <= 0) {
        return os;
    }
    std::wstring u16String(u16Length, 0);
    ::MultiByteToWideChar(CP_UTF8, 0, (char const *)s.data(), s.size(), u16String.data(), u16Length);

    int acpLength = ::WideCharToMultiByte(CP_THREAD_ACP, 0, u16String.data(), u16String.size(), nullptr, 0, nullptr, nullptr);
    if (acpLength <= 0) {
        return os;
    }
    std::string acpString(acpLength, 0);
    ::WideCharToMultiByte(CP_THREAD_ACP, 0, u16String.data(), u16String.size(), acpString.data(), acpLength, nullptr, nullptr);

    os << acpString;
    return os;
#else
    std::string ns;
    ns.assign((char const *)s.data(), s.size());
    os << ns;
    return os;
#endif
}

inline std::ostream &operator<<(std::ostream &os, char8_t const *s) {
    std::u8string ss;
    ss.assign(s);
    os << ss;
    return os;
}

} // namespace mcfile::u8stream
