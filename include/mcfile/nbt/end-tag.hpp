#pragma once

namespace mcfile {
namespace nbt {

class EndTag : public Tag {
public:
    bool readImpl(::mcfile::stream::InputStreamReader&) override { return true; }
    void writeImpl(::mcfile::stream::OutputStreamWriter& w) const override {}
    uint8_t id() const override { return TAG_End; }
    std::shared_ptr<Tag> clone() const override { return Shared(); }

    static EndTag const* Instance() {
        return EndTag::Shared().get();
    }

private:
    static std::shared_ptr<EndTag> Shared() {
        static std::shared_ptr<EndTag> s = std::make_shared<EndTag>();
        return s;
    }
};

} // namespace nbt
} // namespace mcfile
