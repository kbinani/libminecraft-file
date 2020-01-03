#pragma once

namespace mcfile {
namespace nbt {

inline std::shared_ptr<Tag> TagFactory::makeTag(uint8_t id, std::string const& name) {
    switch (id) {
        case Tag::TAG_End:
            return makeNamedTag<EndTag>(name);
        case Tag::TAG_Byte:
            return makeNamedTag<ByteTag>(name);
        case Tag::TAG_Short:
            return makeNamedTag<ShortTag>(name);
        case Tag::TAG_Int:
            return makeNamedTag<IntTag>(name);
        case Tag::TAG_Long:
            return makeNamedTag<LongTag>(name);
        case Tag::TAG_Float:
            return makeNamedTag<FloatTag>(name);
        case Tag::TAG_Double:
            return makeNamedTag<DoubleTag>(name);
        case Tag::TAG_Byte_Array:
            return makeNamedTag<ByteArrayTag>(name);
        case Tag::TAG_String:
            return makeNamedTag<StringTag>(name);
        case Tag::TAG_List:
            return makeNamedTag<ListTag>(name);
        case Tag::TAG_Compound:
            return makeNamedTag<CompoundTag>(name);
        case Tag::TAG_Int_Array:
            return makeNamedTag<IntArrayTag>(name);
        case Tag::TAG_Long_Array:
            return makeNamedTag<LongArrayTag>(name);
        default:
            return nullptr;
    }
    return nullptr;
}

} // namespace nbt
} // namespace mcfile
