#pragma once

namespace mcfile {
namespace stream {

class InputStream {
public:
    InputStream() = default;

    InputStream(InputStream const &) = delete;

    InputStream &operator=(InputStream const &) = delete;

    virtual ~InputStream() {}

    virtual uint64_t length() const = 0;

    virtual bool read(void *buffer, size_t size, size_t count) = 0;

    virtual bool seek(uint64_t offset) = 0;

    virtual bool valid() const = 0;

    virtual uint64_t pos() const = 0;
};

} // namespace stream
} // namespace mcfile
