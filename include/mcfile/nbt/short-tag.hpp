#pragma once

namespace mcfile {
namespace nbt {

class ShortTag : public detail::ScalarTag<int16_t, Tag::Type::Short> {
public:
    ShortTag()
        : ScalarTag() {}
    explicit ShortTag(int16_t v)
        : ScalarTag(v) {}

    std::shared_ptr<ShortTag> copy() const {
        return std::make_shared<ShortTag>(fValue);
    }
};

} // namespace nbt
} // namespace mcfile
