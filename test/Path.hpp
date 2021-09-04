#pragma once

#include <unistd.h>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#else
#include <linux/limits.h>
#endif
#include <algorithm>
#include <libgen.h>
#include <string>
#include <vector>

class Path {
public:
    Path() = delete;

    static std::string CurrentExecutablePath() {
        std::vector<char> data(PATH_MAX);
#if defined(__APPLE__)
        uint32_t len = data.size();
        _NSGetExecutablePath(data.data(), &len);
#else
        readlink("/proc/self/exe", data.data(), data.size() - 1);
#endif

        return Path::Realpath(std::string(data.data()));
    }

    static std::string Realpath(std::string const &path) {
        std::vector<char> buffer(std::max((size_t)PATH_MAX, path.size() + 1));
        std::copy(path.begin(), path.end(), buffer.begin());
        std::vector<char> copy = buffer;
        ::realpath(buffer.data(), copy.data());
        return std::string(copy.data());
    }

    static std::string Dirname(std::string const &path) {
        std::vector<char> buffer(path.size());
        std::copy(path.begin(), path.end(), buffer.begin());
#if defined(__APPLE__)
        ::dirname_r(path.c_str(), buffer.data());
#else
        ::dirname(buffer.data());
#endif
        return std::string(buffer.data());
    }
};
