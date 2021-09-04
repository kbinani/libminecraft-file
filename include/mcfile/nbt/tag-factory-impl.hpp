#pragma once

namespace mcfile {
namespace nbt {

inline std::shared_ptr<Tag> TagFactory::makeTag(uint8_t id, std::string const &name) {
    switch (id) {
    case static_cast<uint8_t>(Tag::Type::End):
        return makeNamedTag<EndTag>(name);
    case static_cast<uint8_t>(Tag::Type::Byte):
        return makeNamedTag<ByteTag>(name);
    case static_cast<uint8_t>(Tag::Type::Short):
        return makeNamedTag<ShortTag>(name);
    case static_cast<uint8_t>(Tag::Type::Int):
        return makeNamedTag<IntTag>(name);
    case static_cast<uint8_t>(Tag::Type::Long):
        return makeNamedTag<LongTag>(name);
    case static_cast<uint8_t>(Tag::Type::Float):
        return makeNamedTag<FloatTag>(name);
    case static_cast<uint8_t>(Tag::Type::Double):
        return makeNamedTag<DoubleTag>(name);
    case static_cast<uint8_t>(Tag::Type::ByteArray):
        return makeNamedTag<ByteArrayTag>(name);
    case static_cast<uint8_t>(Tag::Type::String):
        return makeNamedTag<StringTag>(name);
    case static_cast<uint8_t>(Tag::Type::List):
        return makeNamedTag<ListTag>(name);
    case static_cast<uint8_t>(Tag::Type::Compound):
        return makeNamedTag<CompoundTag>(name);
    case static_cast<uint8_t>(Tag::Type::IntArray):
        return makeNamedTag<IntArrayTag>(name);
    case static_cast<uint8_t>(Tag::Type::LongArray):
        return makeNamedTag<LongArrayTag>(name);
    default:
        return nullptr;
    }
    return nullptr;
}

} // namespace nbt
} // namespace mcfile
