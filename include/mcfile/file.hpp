#pragma once

namespace mcfile {

class File {
private:
    File() = delete;

public:
    enum class Mode {
        Read,
        Write,
        Append,
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

    [[nodiscard]] static bool Fseek(FILE *file, uint64_t pos, int origin) {
#if defined(_WIN32)
        return _fseeki64(file, pos, origin) == 0;
#else
        return fseeko(file, pos, origin) == 0;
#endif
    }

    static std::optional<uint64_t> Ftell(FILE *file) {
#if defined(_WIN32)
        auto ret = _ftelli64(file);
#else
        auto ret = ftello(file);
#endif
        if (ret < 0) {
            return std::nullopt;
        } else {
            return static_cast<uint64_t>(ret);
        }
    }

    [[nodiscard]] static bool Fwrite(void const *buffer, size_t elementSize, size_t elementCount, FILE *stream) {
        return fwrite(buffer, elementSize, elementCount, stream) == elementCount;
    }

    [[nodiscard]] static bool Fread(void *buffer, size_t elementSize, size_t elementCount, FILE *stream) {
        return fread(buffer, elementSize, elementCount, stream) == elementCount;
    }

    [[nodiscard]] static bool Copy(FILE *in, FILE *out, size_t length) {
        if (!in || !out) {
            return false;
        }
        if (length == 0) {
            return true;
        }
        uint8_t buffer[512] = {0};
        size_t const size = sizeof(buffer[0]);
        size_t const nitems = sizeof(buffer) / size;
        size_t remain = length;
        while (remain > 0) {
            size_t const num = std::min(nitems, remain);
            size_t const read = fread(buffer, size, num, in);
            if (read != num) {
                return false;
            }
            size_t const written = fwrite(buffer, size, read, out);
            if (written != read) {
                return false;
            }
            assert(remain >= read);
            remain -= read;
        }
        return true;
    }

    static std::optional<std::filesystem::path> CreateTempDir(std::filesystem::path const &tempDir) {
        using namespace std;
        namespace fs = std::filesystem;
#if defined(_MSC_VER)
        while (true) {
            random_device rd;
            mt19937_64 mt(rd());
            uint64_t rnd = mt();
            auto candidate = tempDir / ("mcfile-tmp-" + to_string(rnd));
            error_code ec;
            fs::create_directories(candidate, ec);
            if (!ec) {
                return candidate;
            }
        }
#else
        string tmpl = (tempDir / "mcfile-tmp-XXXXXX").string();
        vector<char> buffer;
        copy(tmpl.begin(), tmpl.end(), back_inserter(buffer));
        buffer.push_back(0);
        char *dir = mkdtemp(buffer.data());
        if (dir) {
            return string(dir, strlen(dir));
        } else {
            return nullopt;
        }
#endif
    }

private:
    static std::string ModeString(Mode mode) {
        switch (mode) {
        case Mode::Read:
            return "rb";
        case Mode::Write:
            return "wb";
        case Mode::Append:
            return "ab";
        }
        return "rb";
    }

    static std::wstring ModeWString(Mode mode) {
        switch (mode) {
        case Mode::Read:
            return L"rb";
        case Mode::Write:
            return L"wb";
        case Mode::Append:
            return L"ab";
        }
        return L"rb";
    }
};

} // namespace mcfile
