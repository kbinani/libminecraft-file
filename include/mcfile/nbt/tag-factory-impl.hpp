#pragma once

namespace mcfile {
namespace nbt {

inline std::shared_ptr<Tag> TagFactory::makeTag(uint8_t id) {
    switch (id) {
    case static_cast<uint8_t>(Tag::Type::End):
        return makeTagImpl<EndTag>();
    case static_cast<uint8_t>(Tag::Type::Byte):
        return makeTagImpl<ByteTag>();
    case static_cast<uint8_t>(Tag::Type::Short):
        return makeTagImpl<ShortTag>();
    case static_cast<uint8_t>(Tag::Type::Int):
        return makeTagImpl<IntTag>();
    case static_cast<uint8_t>(Tag::Type::Long):
        return makeTagImpl<LongTag>();
    case static_cast<uint8_t>(Tag::Type::Float):
        return makeTagImpl<FloatTag>();
    case static_cast<uint8_t>(Tag::Type::Double):
        return makeTagImpl<DoubleTag>();
    case static_cast<uint8_t>(Tag::Type::ByteArray):
        return makeTagImpl<ByteArrayTag>();
    case static_cast<uint8_t>(Tag::Type::String):
        return makeTagImpl<StringTag>();
    case static_cast<uint8_t>(Tag::Type::List):
        return makeTagImpl<ListTag>();
    case static_cast<uint8_t>(Tag::Type::Compound):
        return makeTagImpl<CompoundTag>();
    case static_cast<uint8_t>(Tag::Type::IntArray):
        return makeTagImpl<IntArrayTag>();
    case static_cast<uint8_t>(Tag::Type::LongArray):
        return makeTagImpl<LongArrayTag>();
    default:
        return nullptr;
    }
    return nullptr;
}

} // namespace nbt
} // namespace mcfile
