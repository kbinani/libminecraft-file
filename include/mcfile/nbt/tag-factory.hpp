#pragma once

namespace mcfile::nbt {

class Tag;

class TagFactory {
public:
    static inline std::shared_ptr<Tag> makeTag(uint8_t id);

private:
    template<typename T>
    static std::shared_ptr<T> makeTagImpl() {
        struct U : T {
            U()
                : T() {}
        };
        return std::make_shared<U>();
    }
};

} // namespace mcfile::nbt
