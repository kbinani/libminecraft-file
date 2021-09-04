#pragma once

namespace mcfile {
namespace nbt {

class Tag;

class TagFactory {
public:
    static inline std::shared_ptr<Tag> makeTag(uint8_t id, std::string const &name);

private:
    template<typename T>
    static std::shared_ptr<T> makeNamedTag(std::string const &name) {
        auto p = std::shared_ptr<T>(new T);
        p->fName = name;
        return p;
    }
};

} // namespace nbt
} // namespace mcfile
