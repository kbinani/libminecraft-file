#pragma once

namespace mcfile {
namespace nbt {

class EndTag;
class ByteTag;
class ShortTag;
class IntTag;
class LongTag;
class FloatTag;
class DoubleTag;
class ByteArrayTag;
class StringTag;
class ListTag;
class CompoundTag;
class IntArrayTag;
class LongArrayTag;

class Tag {
public:
    friend class TagFactory;

    enum class Type : uint8_t {
        End = 0,
        Byte = 1,
        Short = 2,
        Int = 3,
        Long = 4,
        Float = 5,
        Double = 6,
        ByteArray = 7,
        String = 8,
        List = 9,
        Compound = 10,
        IntArray = 11,
        LongArray = 12,
    };

    enum class TypeLimits : uint8_t {
        Min = static_cast<uint8_t>(Type::End),
        Max = static_cast<uint8_t>(Type::LongArray),
    };

    Tag()
        : fValid(false) {}
    virtual ~Tag() {}

    Tag(Tag const &) = delete;
    Tag &operator=(Tag const &) = delete;

    void read(::mcfile::stream::InputStreamReader &reader) {
        fValid = readImpl(reader);
    }

    void write(::mcfile::stream::OutputStreamWriter &writer) const {
        writeImpl(writer);
    }

    bool valid() const { return fValid; }

    virtual Type type() const = 0;

    std::string name() const { return fName; }

    EndTag const *asEnd() const { return this->type() == Type::End ? reinterpret_cast<EndTag const *>(this) : nullptr; }
    ByteTag const *asByte() const { return this->type() == Type::Byte ? reinterpret_cast<ByteTag const *>(this) : nullptr; }
    ShortTag const *asShort() const { return this->type() == Type::Short ? reinterpret_cast<ShortTag const *>(this) : nullptr; }
    IntTag const *asInt() const { return this->type() == Type::Int ? reinterpret_cast<IntTag const *>(this) : nullptr; }
    LongTag const *asLong() const { return this->type() == Type::Long ? reinterpret_cast<LongTag const *>(this) : nullptr; }
    FloatTag const *asFloat() const { return this->type() == Type::Float ? reinterpret_cast<FloatTag const *>(this) : nullptr; }
    DoubleTag const *asDouble() const { return this->type() == Type::Double ? reinterpret_cast<DoubleTag const *>(this) : nullptr; }
    ByteArrayTag const *asByteArray() const { return this->type() == Type::ByteArray ? reinterpret_cast<ByteArrayTag const *>(this) : nullptr; }
    StringTag const *asString() const { return this->type() == Type::String ? reinterpret_cast<StringTag const *>(this) : nullptr; }
    ListTag const *asList() const { return this->type() == Type::List ? reinterpret_cast<ListTag const *>(this) : nullptr; }
    CompoundTag const *asCompound() const { return this->type() == Type::Compound ? reinterpret_cast<CompoundTag const *>(this) : nullptr; }
    IntArrayTag const *asIntArray() const { return this->type() == Type::IntArray ? reinterpret_cast<IntArrayTag const *>(this) : nullptr; }
    LongArrayTag const *asLongArray() const { return this->type() == Type::LongArray ? reinterpret_cast<LongArrayTag const *>(this) : nullptr; }

    virtual std::shared_ptr<Tag> clone() const = 0;

protected:
    virtual bool readImpl(::mcfile::stream::InputStreamReader &reader) = 0;
    virtual void writeImpl(::mcfile::stream::OutputStreamWriter &writer) const = 0;

protected:
    std::string fName;
    bool fValid;
};

} // namespace nbt
} // namespace mcfile
