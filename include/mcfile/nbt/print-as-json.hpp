#pragma once

namespace mcfile {
namespace nbt {

struct JsonPrintOptions {
    bool fTypeHint = false;
    int fIndent = 2;
    bool fNewLine = true;
};

namespace detail {

static inline std::string Indent(int level, int size) {
    return std::string((size_t)std::max(0, level) * (size_t)std::max(0, size), ' ');
}

template<class Stream>
static inline void PrintAsJsonImpl(Stream &out, mcfile::nbt::Tag const &value, JsonPrintOptions options, bool comma = false, int depth = 0) {
    using namespace mcfile::u8stream;
    std::string hint = "";
    std::string endl = options.fNewLine ? "\n" : "";
    if (!options.fNewLine && options.fTypeHint) {
        throw std::invalid_argument("Invalid argument combination: fNewLine=false and fTypeHint=true");
    }
    switch (value.type()) {
    case Tag::Type::Byte: {
        uint8_t u8 = value.asByte()->fValue;
        int8_t i8 = *(int8_t *)&u8;
        out << (int)i8;
        hint = "byte";
        break;
    }
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
        if (compound->empty()) {
            out << "{}";
        } else {
            out << "{" << endl;
            for (auto it = compound->begin(); it != compound->end();) {
                std::u8string name = it->first;
                out << Indent(depth + 1, options.fIndent) << "\"" << name << "\": ";
                std::shared_ptr<Tag> value = it->second;
                it++;
                PrintAsJsonImpl(out, *value, options, it != compound->end(), depth + 1);
            }
            out << Indent(depth, options.fIndent) << "}";
        }
        break;
    }
    case Tag::Type::List: {
        auto list = value.asList();
        if (list->empty()) {
            out << "[]";
            hint = "list";
        } else {
            out << "[" << endl;
            for (int i = 0; i < list->fValue.size(); i++) {
                out << Indent(depth + 1, options.fIndent);
                PrintAsJsonImpl(out, *list->fValue[i], options, i + 1 < list->fValue.size(), depth + 1);
            }
            out << Indent(depth, options.fIndent) << "]";
        }
        break;
    }
    case Tag::Type::ByteArray: {
        auto const &list = value.asByteArray()->value();
        if (list.empty()) {
            out << "[]";
            hint = "byte[]";
        } else {
            out << "[";
            if (options.fTypeHint) {
                out << " // byte[]";
            }
            out << endl;
            for (int i = 0; i < list.size(); i++) {
                uint8_t u8 = list[i];
                int8_t i8 = *(int8_t *)&u8;
                out << Indent(depth + 1, options.fIndent) << (int)i8 << endl;
            }
            out << Indent(depth, options.fIndent) << "]";
        }
        break;
    }
    case Tag::Type::IntArray: {
        auto const &list = value.asIntArray()->value();
        if (list.empty()) {
            out << "[]";
            hint = "int[]";
        } else {
            out << "[";
            if (options.fTypeHint) {
                out << " // int[]";
            }
            out << endl;
            for (int i = 0; i < list.size(); i++) {
                out << Indent(depth + 1, options.fIndent) << list[i] << endl;
            }
            out << Indent(depth, options.fIndent) << "]";
        }
        break;
    }
    case Tag::Type::LongArray: {
        auto const &list = value.asLongArray()->value();
        if (list.empty()) {
            out << "[]";
            hint = "long[]";
        } else {
            out << "[";
            if (options.fTypeHint) {
                out << " // long[]";
            }
            out << endl;
            for (int i = 0; i < list.size(); i++) {
                out << Indent(depth + 1, options.fIndent) << list[i] << endl;
            }
            out << Indent(depth, options.fIndent) << "]";
        }
        break;
    }
    case Tag::Type::End:
        break;
    }

    if (comma) {
        out << ",";
    }
    if (options.fTypeHint && !hint.empty()) {
        out << " // " << hint;
    }
    out << endl;
}
} // namespace detail

template<class Stream>
static inline void PrintAsJson(Stream &out, mcfile::nbt::Tag const &tag, JsonPrintOptions options = {}) {
    detail::PrintAsJsonImpl(out, tag, options, false, 0);
}

} // namespace nbt
} // namespace mcfile
