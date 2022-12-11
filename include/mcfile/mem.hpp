#pragma once

namespace mcfile {

class Mem {
    Mem() = delete;
public:
    template <class T>
    static inline T Read(std::string const& buffer, size_t off) {
        assert(off + sizeof(T) <= buffer.size());
        T v;
        buffer.copy((char *)&v, sizeof(v), off);
        return v;
    }
    
    template <class T, class Element, class = std::enable_if_t<sizeof(Element) == 1>>
    static inline T Read(std::vector<Element> const& buffer, size_t off) {
        assert(off + sizeof(T) <= buffer.size());
        T v;
        memcpy((void *)&v, buffer.data() + off, sizeof(v));
        return v;
    }
    
    template <class T>
    static inline void Write(std::string &buffer, size_t off, T v) {
        assert(off + sizeof(T) <= buffer.size());
        memcpy(buffer.data() + off, (void *)&v, sizeof(v));
    }
    
    template <class T, class Element, class = std::enable_if_t<sizeof(Element) == 1>>
    static inline void Write(std::vector<Element> &buffer, size_t off, T v) {
        assert(off + sizeof(T) <= buffer.size());
        memcpy(buffer.data() + off, (void *)&v, sizeof(v));
    }
};

}
