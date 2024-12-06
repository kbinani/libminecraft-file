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
        string tmp;
        for (size_t i = 0; i < s.size(); i++) {
            char c = s[i];
            if (c == '\xc0' && i + 1 < s.size() && s[i + 1] == '\x80') {
                tmp += '\0';
                i++;
            } else {
                tmp += c;
            }
        }
        return ConvertImpl("CESU8", "UTF8", tmp);
    }

    static std::optional<std::string> JavaUtf8FromUtf8(std::u8string_view const &s) {
        using namespace std;
        auto tmp = ConvertImpl("UTF8", "CESU8", s);
        if (!tmp) {
            return nullopt;
        }
        string result;
        for (char8_t ch : *tmp) {
            if (ch == u8'\0') {
                result += '\xc0';
                result += '\x80';
            } else {
                result += (char)ch;
            }
        }
        return result;
    }

    static std::optional<std::u8string> ValidateUtf8(std::u8string const &s, bool allowUnprintable) {
        UErrorCode err = U_ZERO_ERROR;
        UConverter *conv = ucnv_open("UTF8", &err);
        if (U_FAILURE(err)) {
            return std::nullopt;
        }
        struct Closer {
            explicit Closer(UConverter *cnv)
                : cnv(cnv) {}
            UConverter *cnv;
            ~Closer() {
                ucnv_close(cnv);
            }
        } closer(conv);
        size_t offset = 0;
        std::vector<UChar> buf(128);
        while (offset < s.size()) {
            UChar *out = buf.data();
            char const *in = (char const *)s.c_str() + offset;
            err = U_ZERO_ERROR;
            ucnv_toUnicode(conv, &out, (UChar const *)buf.data() + buf.size(), &in, (char const *)s.c_str() + s.size(), nullptr, false, &err);
            if (err != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(err)) {
                return std::nullopt;
            }
            offset += in - ((char const *)s.c_str() + offset);
        }
        return s;
    }

private:
    template<class StringView>
    static std::optional<std::u8string> ConvertImpl(char const *from, char const *to, StringView const &input) {
        using namespace std;
        struct Closer {
            ~Closer() {
                func();
            }
            explicit Closer(std::function<void()> func)
                : func(func) {}

            std::function<void()> func;
        };
        UErrorCode err = U_ZERO_ERROR;
        auto fromConverter = ucnv_open(from, &err);
        if (U_FAILURE(err) || !fromConverter) {
            return nullopt;
        }
        Closer c0([&]() {
            ucnv_close(fromConverter);
        });
        err = U_ZERO_ERROR;
        auto toConverter = ucnv_open(to, &err);
        if (U_FAILURE(err) || !toConverter) {
            return nullopt;
        }
        Closer c1([&]() {
            ucnv_close(toConverter);
        });
        vector<UChar> unicodes;
        vector<UChar> bufu(128);
        size_t offset = 0;
        while (offset < input.size()) {
            err = U_ZERO_ERROR;
            UChar *out = bufu.data();
            char const *in = (char const *)input.data() + offset;
            ucnv_toUnicode(fromConverter, &out, bufu.data() + bufu.size(), &in, (char const *)input.data() + input.size(), nullptr, false, &err);
            if (err != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(err)) {
                return nullopt;
            }
            offset += in - ((char const *)input.data() + offset);
            size_t count = out - bufu.data();
            copy_n(bufu.data(), count, back_inserter(unicodes));
        }

        u8string result;
        vector<char> bufo(128);
        offset = 0;
        while (offset < unicodes.size()) {
            err = U_ZERO_ERROR;
            char *out = bufo.data();
            UChar const *in = unicodes.data() + offset;
            ucnv_fromUnicode(toConverter, &out, bufo.data() + bufo.size(), &in, (UChar const *)unicodes.data() + unicodes.size(), nullptr, false, &err);
            if (err != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(err)) {
                return nullopt;
            }
            offset += in - ((UChar const *)unicodes.data() + offset);
            size_t count = out - bufo.data();
            for (size_t i = 0; i < count; i++) {
                result += (char8_t)bufo[i];
            }
        }
        return result;
    }
};

} // namespace mcfile
