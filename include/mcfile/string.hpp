#pragma once

namespace mcfile {

class String {
public:
    String() = delete;

    static std::vector<std::u8string> Split(std::u8string const &sentence, char8_t delimiter) {
        std::basic_istringstream<char8_t> input(sentence);
        std::vector<std::u8string> tokens;
        for (std::u8string line; std::getline(input, line, delimiter);) {
            tokens.push_back(line);
        }
        return tokens;
    }

    static std::vector<std::string> Split(std::string const &sentence, char delimiter) {
        std::istringstream input(sentence);
        std::vector<std::string> tokens;
        for (std::string line; std::getline(input, line, delimiter);) {
            tokens.push_back(line);
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
