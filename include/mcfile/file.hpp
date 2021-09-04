#pragma once

namespace mcfile {

class File {
private:
    File() = delete;

public:
    enum class Mode {
        Read,
        Write,
    };

    static FILE *Open(std::string const &path, Mode mode) = delete;

    static FILE *Open(std::filesystem::path const &path, Mode mode) {
#if defined(_WIN32)
        auto m = ModeWString(mode);
        FILE *ret = nullptr;
        if (_wfopen_s(&ret, path.c_str(), m.c_str()) == 0) {
            return ret;
        } else {
            return nullptr;
        }
#else
        auto m = ModeString(mode);
        return fopen(path.c_str(), m.c_str());
#endif
    }

    static gzFile GzOpen(std::string const &path, Mode mode) = delete;

    static gzFile GzOpen(std::filesystem::path const &path, Mode mode) {
        auto m = ModeString(mode);
        auto p = path.c_str();
#if defined(_WIN32)
        return gzopen_w(p, m.c_str());
#else
        return gzopen(p, m.c_str());
#endif
    }

private:
    static std::string ModeString(Mode mode) {
        switch (mode) {
        case Mode::Read:
            return "rb";
        case Mode::Write:
            return "wb";
        }
        return "rb";
    }

    static std::wstring ModeWString(Mode mode) {
        switch (mode) {
        case Mode::Read:
            return L"rb";
        case Mode::Write:
            return L"wb";
        }
        return L"rb";
    }
};

} // namespace mcfile
