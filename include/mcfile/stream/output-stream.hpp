#pragma once

namespace mcfile {
namespace stream {

class OutputStream {
public:
    OutputStream() = default;

    OutputStream(OutputStream const &) = delete;

    OutputStream &operator=(OutputStream const &) = delete;

    virtual ~OutputStream() {}

    virtual bool write(void const *buffer, size_t size) = 0;

    virtual bool seek(uint64_t offset) = 0;

    virtual uint64_t pos() const = 0;
};

} // namespace stream
} // namespace mcfile
