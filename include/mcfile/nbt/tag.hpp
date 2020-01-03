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

    Tag() : fValid(false) {}
    virtual ~Tag() {}

    Tag(Tag const&) = delete;
    Tag& operator = (Tag const&) = delete;

    void read(::mcfile::detail::StreamReader& reader) {
        fValid = readImpl(reader);
    }

    bool valid() const { return fValid; }

    virtual uint8_t id() const = 0;

    std::string name() const { return fName; }

    EndTag       const* asEnd() const       { return this->id() == TAG_End ? reinterpret_cast<EndTag const*>(this) : nullptr; }
    ByteTag      const* asByte() const      { return this->id() == TAG_Byte ? reinterpret_cast<ByteTag const*>(this) : nullptr; }
    ShortTag     const* asShort() const     { return this->id() == TAG_Short ? reinterpret_cast<ShortTag const*>(this) : nullptr; }
    IntTag       const* asInt() const       { return this->id() == TAG_Int ? reinterpret_cast<IntTag const*>(this) : nullptr; }
    LongTag      const* asLong() const      { return this->id() == TAG_Long ? reinterpret_cast<LongTag const*>(this) : nullptr; }
    FloatTag     const* asFloat() const     { return this->id() == TAG_Float ? reinterpret_cast<FloatTag const*>(this) : nullptr; }
    DoubleTag    const* asDouble() const    { return this->id() == TAG_Double ? reinterpret_cast<DoubleTag const*>(this) : nullptr; }
    ByteArrayTag const* asByteArray() const { return this->id() == TAG_Byte_Array ? reinterpret_cast<ByteArrayTag const*>(this) : nullptr; }
    StringTag    const* asString() const    { return this->id() == TAG_String ? reinterpret_cast<StringTag const*>(this) : nullptr; }
    ListTag      const* asList() const      { return this->id() == TAG_List ? reinterpret_cast<ListTag const*>(this) : nullptr; }
    CompoundTag  const* asCompound() const  { return this->id() == TAG_Compound ? reinterpret_cast<CompoundTag const*>(this) : nullptr; }
    IntArrayTag  const* asIntArray() const  { return this->id() == TAG_Int_Array ? reinterpret_cast<IntArrayTag const*>(this) : nullptr; }
    LongArrayTag const* asLongArray() const { return this->id() == TAG_Long_Array ? reinterpret_cast<LongArrayTag const*>(this) : nullptr; }

protected:
    virtual bool readImpl(::mcfile::detail::StreamReader& reader) = 0;

public:
    static uint8_t const TAG_End = 0;
    static uint8_t const TAG_Byte = 1;
    static uint8_t const TAG_Short = 2;
    static uint8_t const TAG_Int = 3;
    static uint8_t const TAG_Long = 4;
    static uint8_t const TAG_Float = 5;
    static uint8_t const TAG_Double = 6;
    static uint8_t const TAG_Byte_Array = 7;
    static uint8_t const TAG_String = 8;
    static uint8_t const TAG_List = 9;
    static uint8_t const TAG_Compound = 10;
    static uint8_t const TAG_Int_Array = 11;
    static uint8_t const TAG_Long_Array = 12;

protected:
    std::string fName;

private:
    bool fValid;
};

} // namespace nbt
} // namespace mcfile
