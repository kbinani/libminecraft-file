#pragma once

namespace mcfile::nbt {

class EndTag : public Tag, private std::enable_shared_from_this<EndTag> {
public:
    Tag::Type type() const override { return Tag::Type::End; }
    std::shared_ptr<Tag> clone() const override { return Shared()->shared_from_this(); }

    static EndTag const *Instance() {
        return EndTag::Shared();
    }

    void toSnbt(std::ostream &, SnbtOptions const &) const override {}

protected:
    bool readImpl(::mcfile::stream::InputStreamReader &) override { return true; }
    bool writeImpl(::mcfile::stream::OutputStreamWriter &) const override { return true; }

private:
    static EndTag *Shared() {
        static EndTag *const s = new EndTag;
        return s;
    }
};

} // namespace mcfile::nbt
