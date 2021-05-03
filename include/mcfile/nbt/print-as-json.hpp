#pragma once

namespace mcfile {
namespace nbt {
namespace detail {

static inline std::string Indent(int level) {
    return std::string(level * 2, ' ');
}

template<class Stream>
static inline void PrintAsJsonImpl(Stream& out, mcfile::nbt::Tag const& value, bool comma = false, int depth = 0) {
    switch (value.id()) {
    case Tag::TAG_Byte:
        out << (int)value.asByte()->fValue;
        break;
    case Tag::TAG_Int:
        out << value.asInt()->fValue;
        break;
    case Tag::TAG_Long:
        out << value.asLong()->fValue;
        break;
    case Tag::TAG_Short:
        out << (int)value.asShort()->fValue;
        break;
    case Tag::TAG_Double:
        out << value.asDouble()->fValue;
        break;
    case Tag::TAG_Float:
        out << value.asFloat()->fValue;
        break;
    case Tag::TAG_String:
        out << "\"" << value.asString()->fValue << "\"";
        break;
    case Tag::TAG_Compound: {
        auto compound = value.asCompound();
        out << "{" << std::endl;
        for (auto it = compound->begin(); it != compound->end();) {
            std::string name = it->first;
            out << Indent(depth + 1) << "\"" << name << "\": ";
            std::shared_ptr<Tag> value = it->second;
            it++;
            PrintAsJsonImpl(out, *value, it != compound->end(), depth + 1);
        }
        out << Indent(depth) << "}";
        break;
    }
    case Tag::TAG_List: {
        out << "[" << std::endl;
        auto list = value.asList();
        for (int i = 0; i < list->fValue.size(); i++) {
            PrintAsJsonImpl(out, *list->fValue[i], i + 1 < list->fValue.size(), depth + 1);
        }
        out << Indent(depth) << "]";
        break;
    }
    case Tag::TAG_Byte_Array: {
        out << "[" << std::endl;
        auto const& list = value.asByteArray()->value();
        for (int i = 0; i < list.size(); i++) {
            out << Indent(depth + 1) << (int)list[i] << std::endl;
        }
        out << Indent(depth) << "]";
        break;
    }
    case Tag::TAG_Int_Array: {
        out << "[" << std::endl;
        auto const& list = value.asIntArray()->value();
        for (int i = 0; i < list.size(); i++) {
            out << Indent(depth + 1) << list[i] << std::endl;
        }
        out << Indent(depth) << "]";
        break;
    }
    case Tag::TAG_Long_Array: {
        out << "[" << std::endl;
        auto const& list = value.asLongArray()->value();
        for (int i = 0; i < list.size(); i++) {
            out << Indent(depth + 1) << list[i] << std::endl;
        }
        out << Indent(depth) << "]";
        break;
    }
    }

    if (comma) {
        out << ",";
    }
    out << std::endl;
}
} // namespace detail

template<class Stream>
static inline void PrintAsJson(Stream& out, mcfile::nbt::Tag const& tag) {
    detail::PrintAsJsonImpl(out, tag, false, 0);
}

} // namespace nbt
} // namespace mcfile
