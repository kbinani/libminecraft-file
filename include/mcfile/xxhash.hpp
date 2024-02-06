#pragma once

namespace mcfile {

template<class T, class = std::enable_if_t<std::is_same<T, uint32_t>::value || std::is_same<T, int32_t>::value || std::is_same<T, uint64_t>::value || std::is_same<T, int64_t>::value>>
class XXHash {
    using State = std::conditional_t<sizeof(T) == 4, XXH32_state_t, XXH64_state_t>;

public:
    explicit XXHash(T seed = 0)
        : fState(Create()) {
        if (!fState) {
            throw std::bad_alloc();
        }
        Reset(fState, seed);
    }

    ~XXHash() {
        Free(fState);
    }

    void reset(T seed) {
        Reset(fState, seed);
    }

    void update(void const *p, size_t size) {
        Update(fState, p, size);
    }

    T digest() const {
        return Digest(fState);
    }

    static T Digest(void const *p, size_t size, T seed = 0) {
        XXHash<T> h(seed);
        h.update(p, size);
        return h.digest();
    }

private:
    XXHash(XXHash const &) = delete;
    XXHash &operator=(XXHash const &) = delete;

    static State *Create() {
        if constexpr (sizeof(T) == 4) {
            return XXH32_createState();
        } else {
            return XXH64_createState();
        }
    }

    static void Reset(State *s, T seed) {
        if constexpr (sizeof(T) == 4) {
            XXH32_reset(s, seed);
        } else {
            XXH64_reset(s, seed);
        }
    }

    static void Free(State *s) {
        if constexpr (sizeof(T) == 4) {
            XXH32_freeState(s);
        } else {
            XXH64_freeState(s);
        }
    }

    static void Update(State *s, void const *p, size_t size) {
        if constexpr (sizeof(T) == 4) {
            XXH32_update(s, p, size);
        } else {
            XXH64_update(s, p, size);
        }
    }

    static T Digest(State *s) {
        if constexpr (sizeof(T) == 4) {
            auto h = XXH32_digest(s);
            return *(T *)&h;
        } else {
            auto h = XXH64_digest(s);
            return *(T *)&h;
        }
    }

private:
    State *const fState;
};

} // namespace mcfile
