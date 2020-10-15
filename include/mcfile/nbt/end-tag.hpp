#pragma once

namespace mcfile {
namespace nbt {

class EndTag : public Tag {
public:
    bool readImpl(::mcfile::stream::InputStreamReader&) override { return true; }
    void writeImpl(::mcfile::stream::OutputStreamWriter& w) const override {}
    uint8_t id() const override { return TAG_End; }

    static EndTag const* instance() {
        static EndTag s;
        return &s;
    }
};

} // namespace nbt
} // namespace mcfile
