#pragma once

namespace mcfile {
namespace stream {

class OutputStream {
public:
    OutputStream() = default;

    OutputStream(OutputStream const &) = delete;

    OutputStream &operator=(OutputStream const &) = delete;

    virtual ~OutputStream() {}

    virtual bool write(void *buffer, size_t size) = 0;

    virtual bool seek(long offset) = 0;

    virtual long pos() const = 0;
};

} // namespace stream
} // namespace mcfile
