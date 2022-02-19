#pragma once

namespace mcfile::stream {

class InputStream {
public:
    InputStream() = default;

    InputStream(InputStream const &) = delete;

    InputStream &operator=(InputStream const &) = delete;

    virtual ~InputStream() {}

    virtual size_t read(void *buffer, size_t size) = 0;

    virtual bool seek(uint64_t offset) = 0;

    virtual bool valid() const = 0;

    virtual uint64_t pos() const = 0;

    static void ReadUntilEos(InputStream &s, std::vector<uint8_t> &buffer, size_t chunk = 512) {
        std::vector<uint8_t> tmp((std::max)((size_t)1, chunk));
        while (true) {
            auto read = s.read(tmp.data(), tmp.size());
            std::copy_n(tmp.begin(), read, std::back_inserter(buffer));
            if (read < tmp.size()) {
                break;
            }
        }
    }
};

} // namespace mcfile::stream
