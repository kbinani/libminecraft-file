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
};

}
}
