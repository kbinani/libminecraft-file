#pragma once

namespace mcfile {
namespace stream {

class InputStream {
public:
    InputStream() = default;

    InputStream(InputStream const &) = delete;

    InputStream &operator=(InputStream const &) = delete;

    virtual ~InputStream() {}

    virtual long length() const = 0;

    virtual bool read(void *buffer, size_t size, size_t count) = 0;

    virtual bool seek(long offset) = 0;

    virtual bool valid() const = 0;

    virtual long pos() const = 0;
};

} // namespace stream
} // namespace mcfile
