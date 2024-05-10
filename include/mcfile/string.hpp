#pragma once

namespace mcfile {

class String {
public:
    String() = delete;

    template<class Char>
    static std::vector<std::basic_string<Char>> Split(std::basic_string<Char> const &sentence, Char delimiter) {
        std::vector<std::basic_string<Char>> tokens;
        typename std::basic_string<Char>::size_type off = 0;
        while (off < std::basic_string<Char>::npos) {
            auto found = sentence.find_first_of(delimiter, off);
            if (found == std::basic_string<Char>::npos) {
                break;
            }
            auto sub = sentence.substr(off, found - off);
            tokens.push_back(sub);
            off = found + 1;
        }
        if (off != std::basic_string<Char>::npos) {
            auto sub = sentence.substr(off);
            tokens.push_back(sub);
        }
        return tokens;
    }

    static bool EndsWith(std::string_view const &s, std::string const &search) {
        if (s.size() < search.size()) {
            return false;
        }
        if (search.empty()) {
            return false;
        }
        auto idx = s.rfind(search);
        if (idx == std::string::npos) {
            return false;
        }
        return idx + search.size() == s.size();
    }

    template<std::integral T>
    static T Toi(std::u8string const &s) {
        int v = 0;
        if (auto ret = std::from_chars((char const *)s.c_str(), (char const *)s.c_str() + s.size(), v); ret.ec == std::errc{}) {
            return v;
        } else {
            return 0;
        }
    }

    template<std::integral T>
    static std::u8string ToString(T i) {
        std::u8string v;
        v.resize(std::numeric_limits<T>::digits10 + 2, (char8_t)0);
        if (auto ret = std::to_chars((char *)v.c_str(), (char *)v.c_str() + v.size(), i); ret.ec == std::errc{}) {
            return std::u8string(v.c_str());
        } else {
            return u8"0";
        }
    }

    template<std::floating_point T>
    static std::u8string ToString(T i) {
        std::u8string v;
        v.resize(std::numeric_limits<T>::max_digits10 + 2, (char8_t)0);
        if (auto ret = std::to_chars((char *)v.c_str(), (char *)v.c_str() + v.size(), i); ret.ec == std::errc{}) {
            return std::u8string(v.c_str());
        } else {
            return u8"0";
        }
    }

    static std::optional<std::u8string> Utf8FromJavaUtf8(std::string_view const &s) {
        using namespace std;
        u8string ret;
        size_t i = 0;
        auto at = [s](size_t i) -> uint8_t {
            return *(uint8_t *)(s.data() + i);
        };
        auto append = [&ret](uint8_t c) {
            ret += *(char8_t *)&c;
        };
        while (i < s.size()) {
            uint8_t a = at(i);
            if (a == 0xc0 && i + 1 < s.size() && at(i + 1) == 0x80) {
                append(0);
                i += 2;
            } else if (a < 0x80) {
                append(a);
                i++;
            } else if ((a & 0xe0) == 0xc0) {
                if (i + 1 < s.size()) {
                    append(a);
                    append(at(i + 1));
                    i += 2;
                } else {
                    return nullopt;
                }
            } else if ((a & 0xf0) == 0xe0) {
                if (a == 0xed) {
                    if (i + 5 < s.size()) {
                        auto b = at(i + 1);
                        auto c = at(i + 2);
                        auto d = at(i + 3);
                        auto e = at(i + 4);
                        auto f = at(i + 5);
                        if ((b & 0xf0) != 0xa0 || (c & 0xc0) != 0x80 || d != 0xed || (e & 0xf0) != 0xb0 || (f & 0xc0) != 0x80) {
                            return nullopt;
                        }
                        uint32_t unicode = uint32_t((b & 0xf) + 1) << 16 | (uint32_t(c & 0x3f) << 10) | (uint32_t(e & 0xf) << 6) | uint32_t(f & 0x1f);
                        append(0xf0 | uint8_t(0x7 & (unicode >> 18)));
                        append(0x80 | uint8_t(0x3f & (unicode >> 12)));
                        append(0x80 | uint8_t(0x3f & (unicode >> 6)));
                        append(0x80 | uint8_t(0x3f & unicode));
                        i += 6;
                    } else {
                        return nullopt;
                    }
                } else if (i + 2 < s.size()) {
                    append(a);
                    append(at(i + 1));
                    append(at(i + 2));
                    i += 3;
                } else {
                    return nullopt;
                }
            } else {
                return nullopt;
            }
        }
        return ret;
    }

    static std::optional<std::string> JavaUtf8FromUtf8(std::u8string_view const &s) {
        using namespace std;
        string ret;
        auto at = [&s](size_t i) -> uint8_t {
            return *(uint8_t *)(s.data() + i);
        };
        auto append = [&ret](uint8_t v) {
            ret += *(char *)&v;
        };
        size_t i = 0;
        while (i < s.size()) {
            uint8_t a = at(i);
            if (a == 0) {
                append(0xc0);
                append(0x80);
                i++;
            } else if (a < 0x80) {
                append(a);
                i++;
            } else if ((a & 0xe0) == 0xc0) {
                if (i + 1 < s.size()) {
                    append(a);
                    append(at(i + 1));
                    i += 2;
                } else {
                    return nullopt;
                }
            } else if ((a & 0xf0) == 0xe0) {
                if (i + 2 < s.size()) {
                    append(a);
                    append(at(i + 1));
                    append(at(i + 2));
                    i += 3;
                } else {
                    return nullopt;
                }
            } else if ((a & 0xf8) == 0xf0) {
                if (i + 3 < s.size()) {
                    auto b = at(i + 1);
                    auto c = at(i + 2);
                    auto d = at(i + 3);
                    uint32_t unicode = (uint32_t(a & 0x7) << 18) | (uint32_t(b & 0x3f) << 12) | (uint32_t(c & 0x3f) << 6) | uint32_t(d & 0x3f);
                    uint32_t hi = ((unicode & 0x1f0000) >> 16) - 1;
                    uint32_t lo = unicode & 0xffff;
                    append(0xed);
                    append(0xa0 | uint8_t(hi & 0xf));
                    append(0x80 | uint8_t(0x3f & (lo >> 10)));
                    append(0xed);
                    append(0xb0 | uint8_t(0xf & (lo >> 6)));
                    append(0x80 | uint8_t(0x3f & lo));
                    i += 4;
                } else {
                    return nullopt;
                }
            } else {
                return nullopt;
            }
        }
        return ret;
    }
};

} // namespace mcfile
