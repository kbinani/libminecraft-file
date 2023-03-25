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
};

} // namespace mcfile
