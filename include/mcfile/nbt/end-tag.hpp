#pragma once

namespace mcfile {
namespace nbt {

class EndTag : public Tag, private std::enable_shared_from_this<EndTag> {
public:
    bool readImpl(::mcfile::stream::InputStreamReader &) override { return true; }
    bool writeImpl(::mcfile::stream::OutputStreamWriter &) const override { return true; }
    Tag::Type type() const override { return Tag::Type::End; }
    std::shared_ptr<Tag> clone() const override { return Shared()->shared_from_this(); }

    static EndTag const *Instance() {
        return EndTag::Shared();
    }

private:
    static EndTag *Shared() {
        static EndTag *const s = new EndTag;
        return s;
    }
};

} // namespace nbt
} // namespace mcfile
