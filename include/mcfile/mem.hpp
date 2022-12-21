#pragma once

namespace mcfile {

class Mem {
    Mem() = delete;

public:
    template<class T>
    static inline T Read(std::string const &buffer, size_t off) {
        assert(off + sizeof(T) <= buffer.size());
#if defined(__APPLE__)
        void *p = (void *)(buffer.data() + off);
        void *ptr = p;
        size_t space = buffer.size() - off;
        if (std::align(sizeof(T), sizeof(T), ptr, space) == p) {
            return *(T *)p;
        } else {
            T v;
            buffer.copy((char *)&v, sizeof(v), off);
            return v;
        }
#else
        return *(T *)(buffer.data() + offset);
#endif
    }

    template<class T, class Element, class = std::enable_if_t<sizeof(Element) == 1>>
    static inline T Read(std::vector<Element> const &buffer, size_t off) {
        assert(off + sizeof(T) <= buffer.size());
        void *p = (void *)(buffer.data() + off);
#if defined(__APPLE__)
        void *ptr = p;
        size_t space = buffer.size() - off;
        if (std::align(sizeof(T), sizeof(T), ptr, space) == p) {
            return *(T *)p;
        } else {
            T v;
            memcpy((void *)&v, p, sizeof(v));
            return v;
        }
#else
        return *(T *)(buffer.data() + offset);
#endif
    }

    template<class T>
    static inline void Write(std::string &buffer, size_t off, T v) {
        assert(off + sizeof(T) <= buffer.size());
#if defined(__APPLE__)
        void *p = (void *)(buffer.data() + off);
        void *ptr = p;
        size_t space = buffer.size() - off;
        if (std::align(sizeof(T), sizeof(T), ptr, space) == p) {
            *((T *)p) = v;
        } else {
            memcpy(p, (void *)&v, sizeof(v));
        }
#else
        *(T *)(buffer.data() + offset) = v;
#endif
    }

    template<class T, class Element, class = std::enable_if_t<sizeof(Element) == 1>>
    static inline void Write(std::vector<Element> &buffer, size_t off, T v) {
        assert(off + sizeof(T) <= buffer.size());
#if defined(__APPLE__)
        void *p = (void *)(buffer.data() + off);
        void *ptr = p;
        size_t space = buffer.size() - off;
        if (std::align(sizeof(T), sizeof(T), ptr, space) == p) {
            *((T *)p) = v;
        } else {
            memcpy(p, (void *)&v, sizeof(v));
        }
#else
        *(T *)(buffer.data() + offset) = v;
#endif
    }
};

} // namespace mcfile
