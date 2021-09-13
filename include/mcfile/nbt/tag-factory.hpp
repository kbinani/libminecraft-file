#pragma once

namespace mcfile {
namespace nbt {

class Tag;

class TagFactory {
public:
    static inline std::shared_ptr<Tag> makeTag(uint8_t id);

private:
    template<typename T>
    static std::shared_ptr<T> makeTagImpl() {
        auto p = std::shared_ptr<T>(new T);
        return p;
    }
};

} // namespace nbt
} // namespace mcfile
