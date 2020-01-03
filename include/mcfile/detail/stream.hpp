#pragma once

namespace mcfile {
namespace detail {

class Stream {
public:
    Stream() = default;

    Stream(Stream const &) = delete;

    Stream &operator=(Stream const &) = delete;

    virtual ~Stream() {}

    virtual long length() const = 0;

    virtual bool read(void *buffer, size_t size, size_t count) = 0;

    virtual bool seek(long offset) = 0;

    virtual bool valid() const = 0;

    virtual long pos() const = 0;
};

} // namespace mcfile
} // namespace detail
