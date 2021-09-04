#pragma once

namespace mcfile {
namespace nbt {

struct JsonPrintOptions {
    bool fTypeHint = false;
};

namespace detail {

static inline std::string Indent(int level) {
    return std::string(level * 2, ' ');
}

template<class Stream>
static inline void PrintAsJsonImpl(Stream &out, mcfile::nbt::Tag const &value, JsonPrintOptions options, bool comma = false, int depth = 0) {
    std::string hint = "";
    switch (value.type()) {
    case Tag::Type::Byte:
        out << (int)value.asByte()->fValue;
        hint = "byte";
        break;
    case Tag::Type::Int:
        out << value.asInt()->fValue;
        hint = "int";
        break;
    case Tag::Type::Long:
        out << value.asLong()->fValue;
        hint = "long";
        break;
    case Tag::Type::Short:
        out << (int)value.asShort()->fValue;
        hint = "short";
        break;
    case Tag::Type::Double:
        out << value.asDouble()->fValue;
        hint = "double";
        break;
    case Tag::Type::Float:
        out << value.asFloat()->fValue;
        hint = "float";
        break;
    case Tag::Type::String:
        out << "\"" << value.asString()->fValue << "\"";
        break;
    case Tag::Type::Compound: {
        auto compound = value.asCompound();
        out << "{" << std::endl;
        for (auto it = compound->begin(); it != compound->end();) {
            std::string name = it->first;
            out << Indent(depth + 1) << "\"" << name << "\": ";
            std::shared_ptr<Tag> value = it->second;
            it++;
            PrintAsJsonImpl(out, *value, options, it != compound->end(), depth + 1);
        }
        out << Indent(depth) << "}";
        break;
    }
    case Tag::Type::List: {
        out << "[" << std::endl;
        auto list = value.asList();
        for (int i = 0; i < list->fValue.size(); i++) {
            out << Indent(depth + 1);
            PrintAsJsonImpl(out, *list->fValue[i], options, i + 1 < list->fValue.size(), depth + 1);
        }
        out << Indent(depth) << "]";
        break;
    }
    case Tag::Type::ByteArray: {
        out << "[";
        if (options.fTypeHint) {
            out << " // byte[]";
        }
        out << std::endl;
        auto const &list = value.asByteArray()->value();
        for (int i = 0; i < list.size(); i++) {
            out << Indent(depth + 1) << (int)list[i] << std::endl;
        }
        out << Indent(depth) << "]";
        break;
    }
    case Tag::Type::IntArray: {
        out << "[";
        if (options.fTypeHint) {
            out << " // int[]";
        }
        out << std::endl;
        auto const &list = value.asIntArray()->value();
        for (int i = 0; i < list.size(); i++) {
            out << Indent(depth + 1) << list[i] << std::endl;
        }
        out << Indent(depth) << "]";
        break;
    }
    case Tag::Type::LongArray: {
        out << "[";
        if (options.fTypeHint) {
            out << " // long[]";
        }
        out << std::endl;
        auto const &list = value.asLongArray()->value();
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
    if (options.fTypeHint && !hint.empty()) {
        out << " // " << hint;
    }
    out << std::endl;
}
} // namespace detail

template<class Stream>
static inline void PrintAsJson(Stream &out, mcfile::nbt::Tag const &tag, JsonPrintOptions options = {}) {
    detail::PrintAsJsonImpl(out, tag, options, false, 0);
}

} // namespace nbt
} // namespace mcfile
