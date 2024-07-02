#pragma once

namespace mcfile::nbt {

class CompoundTag : public Tag {
protected:
    bool readImpl(::mcfile::stream::InputStreamReader &r) override {
        std::map<std::u8string, std::shared_ptr<Tag>> tmp;
        while (r.valid()) {
            uint8_t type;
            if (!r.read(&type)) {
                break;
            }
            if (type == static_cast<uint8_t>(Tag::Type::End)) {
                break;
            }

            std::u8string name;
            if (!r.read(name)) {
                return false;
            }

            auto tag = TagFactory::makeTag(type);
            if (!tag) {
                return false;
            }
            if (!tag->readImpl(r)) {
                return false;
            }
            tmp[name] = tag;
        }
        fValue.swap(tmp);
        return true;
    }

    bool writeImpl(::mcfile::stream::OutputStreamWriter &w) const override {
        for (auto it = fValue.begin(); it != fValue.end(); it++) {
            Tag::Type type = it->second->type();
            if (!w.write(static_cast<uint8_t>(type))) {
                return false;
            }
            if (!w.write(it->first)) {
                return false;
            }
            if (!it->second->writeImpl(w)) {
                return false;
            }
        }
        return w.write(static_cast<uint8_t>(Tag::Type::End));
    }

public:
    bool read(::mcfile::stream::InputStreamReader &r) {
        return readImpl(r);
    }

    bool write(::mcfile::stream::OutputStreamWriter &w) const {
        return writeImpl(w);
    }

    [[nodiscard]] static bool Write(CompoundTag const &tag, mcfile::stream::OutputStreamWriter &w) {
        if (!w.write(static_cast<uint8_t>(Tag::Type::Compound))) {
            return false;
        }
        if (!w.write(std::u8string())) {
            return false;
        }
        return tag.writeImpl(w);
    }

    [[nodiscard]] static bool Write(CompoundTag const &tag, std::filesystem::path const &file, Encoding encoding) {
        auto stream = std::make_shared<mcfile::stream::FileOutputStream>(file);
        return Write(tag, stream, encoding);
    }

    [[nodiscard]] static bool Write(CompoundTag const &tag, std::shared_ptr<mcfile::stream::OutputStream> const &stream, Encoding encoding) {
        mcfile::stream::OutputStreamWriter w(stream, encoding);
        return Write(tag, w);
    }

    static std::optional<std::string> Write(CompoundTag const &tag, Encoding encoding) {
        auto s = std::make_shared<mcfile::stream::ByteStream>();
        if (!Write(tag, s, encoding)) {
            return std::nullopt;
        }
        std::string ret;
        s->drain(ret);
        return ret;
    }

    Tag::Type type() const override { return Tag::Type::Compound; }

    Tag const *query(std::u8string const &path) const {
        // path: /Level/Sections
        if (path.empty()) {
            return EndTag::Instance();
        }
        std::u8string p = path;
        Tag const *pivot = this;
        while (!p.empty()) {
            if (p[0] == u8'/') {
                if (fValue.size() != 1) {
                    return EndTag::Instance();
                }
                auto const &child = fValue.begin()->second;
                if (p.size() == 1) {
                    return child.get();
                }
                if (child->type() != Tag::Type::Compound) {
                    return EndTag::Instance();
                }
                pivot = child->asCompound();
                p = p.substr(1);
            } else {
                auto pos = p.find_first_of(u8'/');
                std::u8string name;
                if (pos == std::u8string::npos) {
                    name = p;
                } else {
                    name = p.substr(0, pos);
                }
                if (pivot->type() == Tag::Type::List) {
                    auto list = pivot->asList();
                    if (!list)
                        return EndTag::Instance();
                    int index;
                    try {
                        index = String::Toi<int>(name);
                        if (index < 0 || list->size() <= index) {
                            return EndTag::Instance();
                        }
                        auto const &child = list->at(index);
                        if (pos == std::u8string::npos) {
                            return child.get();
                        }
                        auto id = child->type();
                        if (id != Tag::Type::Compound && id != Tag::Type::List) {
                            return EndTag::Instance();
                        }
                        pivot = child.get();
                        p = p.substr(pos + 1);
                    } catch (...) {
                        return EndTag::Instance();
                    }
                } else if (pivot->type() == Tag::Type::Compound) {
                    auto comp = pivot->asCompound();
                    if (!comp)
                        return EndTag::Instance();

                    auto child = comp->fValue.find(name);
                    if (child == comp->fValue.end()) {
                        return EndTag::Instance();
                    }
                    if (pos == std::u8string::npos) {
                        return child->second.get();
                    }
                    auto id = child->second->type();
                    if (id != Tag::Type::Compound && id != Tag::Type::List) {
                        return EndTag::Instance();
                    }
                    pivot = child->second.get();
                    p = p.substr(pos + 1);
                }
            }
        }
        return EndTag::Instance();
    }

    std::shared_ptr<Tag> &operator[](std::u8string const &name) {
        return fValue[name];
    }

    void insert(std::pair<std::u8string, std::shared_ptr<Tag>> const &item) {
        fValue[item.first] = item.second;
    }

    void insert(std::initializer_list<std::pair<std::u8string const, std::shared_ptr<Tag>>> l) {
        for (auto const &it : l) {
            fValue[it.first] = it.second;
        }
    }

    decltype(auto) find(std::u8string const &name) const { return fValue.find(name); }
    decltype(auto) begin() const { return fValue.begin(); }
    decltype(auto) end() const { return fValue.end(); }
    size_t size() const { return fValue.size(); }
    bool empty() const { return fValue.empty(); }

    void erase(std::u8string const &name) { fValue.erase(name); }

    std::shared_ptr<Tag> tag(std::u8string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end()) {
            return nullptr;
        }
        return found->second;
    }

    std::shared_ptr<StringTag> stringTag(std::u8string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::String)
            return nullptr;
        return std::dynamic_pointer_cast<StringTag>(found->second);
    }

    std::shared_ptr<ByteTag> byteTag(std::u8string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::Byte)
            return nullptr;
        return std::dynamic_pointer_cast<ByteTag>(found->second);
    }

    std::shared_ptr<CompoundTag> compoundTag(std::u8string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::Compound)
            return nullptr;
        return std::dynamic_pointer_cast<CompoundTag>(found->second);
    }

    std::shared_ptr<ListTag> listTag(std::u8string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::List)
            return nullptr;
        return std::dynamic_pointer_cast<ListTag>(found->second);
    }

    std::shared_ptr<IntTag> intTag(std::u8string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::Int)
            return nullptr;
        return std::dynamic_pointer_cast<IntTag>(found->second);
    }

    std::shared_ptr<LongTag> longTag(std::u8string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::Long)
            return nullptr;
        return std::dynamic_pointer_cast<LongTag>(found->second);
    }

    std::shared_ptr<ShortTag> shortTag(std::u8string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::Short)
            return nullptr;
        return std::dynamic_pointer_cast<ShortTag>(found->second);
    }

    std::shared_ptr<FloatTag> floatTag(std::u8string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::Float)
            return nullptr;
        return std::dynamic_pointer_cast<FloatTag>(found->second);
    }

    std::shared_ptr<DoubleTag> doubleTag(std::u8string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::Double)
            return nullptr;
        return std::dynamic_pointer_cast<DoubleTag>(found->second);
    }

    std::shared_ptr<IntArrayTag> intArrayTag(std::u8string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::IntArray)
            return nullptr;
        return std::dynamic_pointer_cast<IntArrayTag>(found->second);
    }

    std::shared_ptr<ByteArrayTag> byteArrayTag(std::u8string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::ByteArray)
            return nullptr;
        return std::dynamic_pointer_cast<ByteArrayTag>(found->second);
    }

    std::shared_ptr<LongArrayTag> longArrayTag(std::u8string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::LongArray)
            return nullptr;
        return std::dynamic_pointer_cast<LongArrayTag>(found->second);
    }

    template<class Tag>
    std::shared_ptr<Tag> get(std::u8string const &name) const;

    template<class Value>
    std::optional<Value> value(std::u8string const &name) const;

    std::optional<int8_t> byte(std::u8string const &name) const {
        auto v = byteTag(name);
        if (v) {
            return v->fValue;
        } else {
            return std::nullopt;
        }
    }

    std::optional<bool> boolean(std::u8string const &name) const {
        auto v = byteTag(name);
        if (v) {
            return v->fValue != 0;
        } else {
            return std::nullopt;
        }
    }

    std::optional<int16_t> int16(std::u8string const &name) const {
        auto v = shortTag(name);
        if (v) {
            return v->fValue;
        } else {
            return std::nullopt;
        }
    }

    std::optional<int32_t> int32(std::u8string const &name) const {
        auto v = intTag(name);
        if (v) {
            return v->fValue;
        } else {
            return std::nullopt;
        }
    }

    std::optional<int64_t> int64(std::u8string const &name) const {
        auto v = longTag(name);
        if (v) {
            return v->fValue;
        } else {
            return std::nullopt;
        }
    }

    std::optional<std::u8string> string(std::u8string const &name) const {
        auto v = stringTag(name);
        if (v) {
            return v->fValue;
        } else {
            return std::nullopt;
        }
    }

    std::optional<float> float32(std::u8string const &name) const {
        auto v = floatTag(name);
        if (v) {
            return v->fValue;
        } else {
            return std::nullopt;
        }
    }

    std::optional<double> float64(std::u8string const &name) const {
        auto v = doubleTag(name);
        if (v) {
            return v->fValue;
        } else {
            return std::nullopt;
        }
    }

    int8_t byte(std::u8string const &name, int8_t fallback) const {
        auto v = byteTag(name);
        return v ? v->fValue : fallback;
    }

    bool boolean(std::u8string const &name, bool fallback) const {
        auto v = byteTag(name);
        return v ? v->fValue != 0 : fallback;
    }

    int16_t int16(std::u8string const &name, int16_t fallback) const {
        auto v = shortTag(name);
        return v ? v->fValue : fallback;
    }

    int32_t int32(std::u8string const &name, int32_t fallback) const {
        auto v = intTag(name);
        return v ? v->fValue : fallback;
    }

    int64_t int64(std::u8string const &name, int64_t fallback) const {
        auto v = longTag(name);
        return v ? v->fValue : fallback;
    }

    std::u8string string(std::u8string const &name, std::u8string const &fallback) const {
        auto v = stringTag(name);
        return v ? v->fValue : fallback;
    }

    float float32(std::u8string const &name, float fallback) const {
        auto v = floatTag(name);
        return v ? v->fValue : fallback;
    }

    double float64(std::u8string const &name, double fallback) const {
        auto v = doubleTag(name);
        return v ? v->fValue : fallback;
    }

    void set(std::u8string const &name, std::shared_ptr<Tag> const &value) {
        fValue[name] = value;
    }

    void set(std::u8string const &name, std::u8string const &value) {
        fValue[name] = std::make_shared<StringTag>(value);
    }

    std::shared_ptr<Tag> clone() const override {
        return copy();
    }

    std::shared_ptr<CompoundTag> copy() const {
        auto ret = std::make_shared<CompoundTag>();
        for (auto const &it : fValue) {
            ret->set(it.first, it.second->clone());
        }
        return ret;
    }

    bool equals(CompoundTag const &o) const {
        using namespace std;
        if (size() != o.size()) {
            return false;
        }
        for (auto const &it : *this) {
            auto tagO = o.tag(it.first);
            if (!tagO) {
                return false;
            }
            if (!Equals(*tagO, *it.second)) {
                return false;
            }
        }
        return true;
    }

    void toSnbt(std::ostream &out, SnbtOptions const &opt) const override {
        out << "{";
        size_t count = 0;
        static std::regex sReg(R"([,:{}\[\]])");
        for (auto const &it : fValue) {
            if (it.second) {
                if (count > 0) {
                    out << ",";
                }
                if (std::regex_search((char const *)it.first.c_str(), sReg)) {
                    out << '"' << (char const *)StringTag::Escape(it.first).c_str() << "\":";
                } else {
                    out << (char const *)StringTag::Escape(it.first).c_str() << ":";
                }
                it.second->toSnbt(out, opt);
                count++;
            }
        }
        out << "}";
    }

    static std::shared_ptr<CompoundTag> FromSnbt(std::istream &in) {
        using namespace std;
        auto root = ReadSnbt(in);
        if (!root) {
            return nullptr;
        }
        return dynamic_pointer_cast<CompoundTag>(root);
    }

    static bool Equals(Tag const &a, Tag const &b) {
        if (a.type() != b.type()) {
            return false;
        }
        switch (a.type()) {
        case Tag::Type::Byte:
            if (!a.asByte()->equals(*b.asByte())) {
                return false;
            }
            break;
        case Tag::Type::ByteArray:
            if (!a.asByteArray()->equals(*b.asByteArray())) {
                return false;
            }
            break;
        case Tag::Type::Compound:
            if (!a.asCompound()->equals(*b.asCompound())) {
                return false;
            }
            break;
        case Tag::Type::Double:
            if (!a.asDouble()->equals(*b.asDouble())) {
                return false;
            }
            break;
        case Tag::Type::End:
            break;
        case Tag::Type::Float:
            if (!a.asFloat()->equals(*b.asFloat())) {
                return false;
            }
            break;
        case Tag::Type::Int:
            if (!a.asInt()->equals(*b.asInt())) {
                return false;
            }
            break;
        case Tag::Type::IntArray:
            if (!a.asIntArray()->equals(*b.asIntArray())) {
                return false;
            }
            break;
        case Tag::Type::List: {
            ListTag const &av = *a.asList();
            ListTag const &bv = *b.asList();
            if (av.size() != bv.size()) {
                return false;
            }
            if (av.fType != bv.fType) {
                return false;
            }
            for (size_t i = 0; i < av.size(); i++) {
                auto const &avi = av.at(i);
                auto const &bvi = bv.at(i);
                if (avi && bvi) {
                    if (!Equals(*avi, *bvi)) {
                        return false;
                    }
                } else {
                    return false;
                }
            }
            break;
        }
        case Tag::Type::Long:
            if (!a.asLong()->equals(*b.asLong())) {
                return false;
            }
            break;
        case Tag::Type::LongArray:
            if (!a.asLongArray()->equals(*b.asLongArray())) {
                return false;
            }
            break;
        case Tag::Type::Short:
            if (!a.asShort()->equals(*b.asShort())) {
                return false;
            }
            break;
        case Tag::Type::String:
            if (!a.asString()->equals(*b.asString())) {
                return false;
            }
            break;
        default:
            return false;
        }
        return true;
    }

    static void ReadSequential(stream::InputStreamReader &reader, std::function<bool(std::shared_ptr<CompoundTag> const &value)> callback) {
        ReadUntilEosImpl(reader, callback);
    }

    static void ReadSequentialUntilEos(stream::InputStreamReader &reader, std::function<void(std::shared_ptr<CompoundTag> const &value)> callback) {
        ReadUntilEosImpl(reader, [callback](std::shared_ptr<CompoundTag> const &value) {
            callback(value);
            return true;
        });
    }

    static void ReadSequential(std::string const &data, Encoding encoding, std::function<bool(std::shared_ptr<CompoundTag> const &value)> callback) {
        auto s = std::make_shared<mcfile::stream::ByteInputStream>(data);
        mcfile::stream::InputStreamReader reader(s, encoding);
        ReadUntilEosImpl(reader, callback);
    }

    static void ReadSequentialUntilEos(std::string const &data, Encoding encoding, std::function<void(std::shared_ptr<CompoundTag> const &value)> callback) {
        auto s = std::make_shared<mcfile::stream::ByteInputStream>(data);
        mcfile::stream::InputStreamReader reader(s, encoding);
        ReadUntilEosImpl(reader, [callback](std::shared_ptr<CompoundTag> const &value) {
            callback(value);
            return true;
        });
    }

    static std::shared_ptr<CompoundTag> ReadFromFile(std::filesystem::path path, Encoding encoding) {
        auto s = std::make_shared<mcfile::stream::FileInputStream>(path);
        return Read(s, encoding);
    }

    static std::shared_ptr<CompoundTag> Read(std::string const &data, Encoding encoding) {
        auto s = std::make_shared<mcfile::stream::ByteInputStream>(data);
        return Read(s, encoding);
    }

    static std::shared_ptr<CompoundTag> Read(std::vector<uint8_t> const &buffer, Encoding encoding) {
        std::vector<uint8_t> tmp;
        tmp.reserve(buffer.size());
        std::copy(buffer.begin(), buffer.end(), std::back_inserter(tmp));
        auto s = std::make_shared<mcfile::stream::ByteStream>(tmp);
        return Read(s, encoding);
    }

    static std::shared_ptr<CompoundTag> Read(std::shared_ptr<stream::InputStream> const &stream, Encoding encoding) {
        mcfile::stream::InputStreamReader isr(stream, encoding);
        return Read(isr);
    }

    static std::shared_ptr<CompoundTag> Read(mcfile::stream::InputStreamReader &isr) {
        uint8_t type;
        if (!isr.read(&type)) {
            return nullptr;
        }
        if (type != static_cast<uint8_t>(Tag::Type::Compound)) {
            return nullptr;
        }
        std::u8string n;
        if (!isr.read(n)) {
            return nullptr;
        }
        assert(n.empty());
        auto tag = std::make_shared<CompoundTag>();
        if (!tag->readImpl(isr)) {
            return nullptr;
        }
        return tag;
    }

    static std::shared_ptr<CompoundTag> ReadCompressedFromFile(std::filesystem::path p, Encoding encoding) {
        auto s = std::make_shared<mcfile::stream::FileInputStream>(p);
        return ReadDeflateCompressed(*s, encoding);
    }

    static std::shared_ptr<CompoundTag> ReadDeflateCompressed(std::string const &data, Encoding encoding) {
        std::vector<uint8_t> buffer;
        buffer.reserve(data.size());
        std::copy(data.begin(), data.end(), std::back_inserter(buffer));
        return ReadDeflateCompressed(buffer, encoding);
    }

    static std::shared_ptr<CompoundTag> ReadDeflateCompressed(mcfile::stream::InputStream &stream, Encoding encoding) {
        std::vector<uint8_t> buffer;
        mcfile::stream::InputStream::ReadUntilEos(stream, buffer);
        return ReadDeflateCompressed(buffer, encoding);
    }

    static std::shared_ptr<CompoundTag> ReadDeflateCompressed(std::vector<uint8_t> const &buffer, Encoding encoding) {
        std::vector<uint8_t> tmp;
        tmp.reserve(buffer.size());
        std::copy(buffer.begin(), buffer.end(), std::back_inserter(tmp));
        if (!Compression::DecompressZlib(tmp)) {
            return nullptr;
        }
        return Read(tmp, encoding);
    }

    static std::shared_ptr<CompoundTag> ReadLz4Compressed(std::string const &data, Encoding encoding) {
        std::vector<uint8_t> buffer;
        buffer.reserve(data.size());
        std::copy(data.begin(), data.end(), std::back_inserter(buffer));
        if (!Compression::DecompressLz4(buffer)) {
            return nullptr;
        }
        return Read(buffer, encoding);
    }

    static std::shared_ptr<CompoundTag> ReadLz4Compressed(mcfile::stream::InputStream &stream, Encoding encoding) {
        std::vector<uint8_t> buffer;
        mcfile::stream::InputStream::ReadUntilEos(stream, buffer);
        if (!Compression::DecompressLz4(buffer)) {
            return nullptr;
        }
        return Read(buffer, encoding);
    }

    static std::shared_ptr<CompoundTag> ReadLz4Compressed(std::vector<uint8_t> const &buffer, Encoding encoding) {
        std::vector<uint8_t> tmp;
        tmp.reserve(buffer.size());
        std::copy(buffer.begin(), buffer.end(), std::back_inserter(tmp));
        if (!Compression::DecompressLz4(tmp)) {
            return nullptr;
        }
        return Read(tmp, encoding);
    }

    static bool WriteDeflateCompressed(CompoundTag const &tag, std::filesystem::path file, Encoding encoding) {
        auto o = std::make_shared<stream::FileOutputStream>(file);
        return WriteDeflateCompressed(tag, *o, encoding);
    }

    static std::optional<std::string> WriteDeflateCompressed(CompoundTag const &tag, Encoding encoding) {
        auto s = std::make_shared<stream::ByteStream>();
        if (!WriteDeflateCompressed(tag, *s, encoding)) {
            return std::nullopt;
        }
        std::string ret;
        s->drain(ret);
        return ret;
    }

    static bool WriteDeflateCompressed(CompoundTag const &tag, mcfile::stream::OutputStream &stream, Encoding encoding) {
        auto s = std::make_shared<stream::ByteStream>();
        stream::OutputStreamWriter osr(s, encoding);
        if (!Write(tag, osr)) {
            return false;
        }
        std::vector<uint8_t> buffer;
        s->drain(buffer);
        if (!Compression::CompressZlib(buffer)) {
            return false;
        }
        return stream.write(buffer.data(), buffer.size());
    }

private:
    static void ReadUntilEosImpl(stream::InputStreamReader &reader, std::function<bool(std::shared_ptr<CompoundTag> const &value)> callback) {
        while (reader.valid()) {
            uint8_t type;
            if (!reader.read(&type)) {
                break;
            }
            std::u8string name;
            if (!reader.read(name)) {
                break;
            }
            auto tag = std::make_shared<CompoundTag>();
            if (!tag->readImpl(reader)) {
                break;
            }
            if (!callback(tag)) {
                break;
            }
        }
    }

    static std::shared_ptr<Tag> ReadSnbt(std::istream &in) {
        using namespace std;
        static auto const sIsDigit = [](char c) {
            return 48 <= c && c <= 57;
        };
        static auto const sReadScalar = [](istream &in) -> shared_ptr<Tag> {
            optional<int> quote;
            int b = in.get();
            if (b == '"' || b == '\'') {
                quote = b;
                b = in.get();
            }
            std::string buffer;
            bool escape = false;
            while (!in.eof()) {
                if (quote) {
                    // quoted string
                    if (quote == b && !escape) {
                        return make_shared<StringTag>(u8string((char8_t const *)buffer.data(), buffer.size()));
                    }
                    if (escape) {
                        buffer.push_back((char)b);
                        escape = false;
                    } else if (b == '\\') {
                        escape = true;
                    } else {
                        buffer.push_back((char)b);
                    }
                } else if (b == ',' || b == ':' || b == ']' || b == '}') {
                    in.unget();
                    break;
                } else {
                    buffer.push_back((char)b);
                }
                b = in.get();
            }
            if (quote) {
                return nullptr;
            }
            if (buffer == "true") {
                return make_shared<ByteTag>(1);
            } else if (buffer == "false") {
                return make_shared<ByteTag>(0);
            }
            bool i8trailing = buffer.ends_with('b') || buffer.ends_with('B');
            bool i16trailing = buffer.ends_with('s') || buffer.ends_with('S');
            bool i64trailing = buffer.ends_with('l') || buffer.ends_with('L');
            bool f32trailing = buffer.ends_with('f') || buffer.ends_with('F');
            bool f64trailing = buffer.ends_with('d') || buffer.ends_with('D');
            if (i8trailing || i16trailing || i64trailing) {
                if (buffer.size() == 1) {
                    return make_shared<StringTag>(u8string((char8_t const *)buffer.data(), buffer.size()));
                }
                if (buffer.starts_with('-')) {
                    if (all_of(buffer.begin() + 1, buffer.begin() + buffer.size() - 1, sIsDigit)) {
                        std::string copy;
                        std::copy(buffer.begin() + 1, buffer.begin() + buffer.size() - 1, back_inserter(copy));
                        char *end = nullptr;
                        uint64_t v = strtoull(copy.c_str(), &end, 10);
                        if (copy.c_str() + copy.size() == end) {
                            if (i8trailing) {
                                auto mini = numeric_limits<int8_t>::lowest();
                                if (-(int64_t)numeric_limits<int8_t>::lowest() < v) {
                                    return nullptr;
                                } else {
                                    int8_t vv = -(int8_t)v;
                                    return make_shared<ByteTag>(*(uint8_t *)&vv);
                                }
                            } else if (i16trailing) {
                                if (-(int64_t)numeric_limits<int16_t>::lowest() < v) {
                                    return nullptr;
                                } else {
                                    return make_shared<ShortTag>(-(int16_t)v);
                                }
                            } else {
                                if (-(int64_t)numeric_limits<int64_t>::lowest() < v) {
                                    return nullptr;
                                } else {
                                    return make_shared<LongTag>(-(int64_t)v);
                                }
                            }
                        }
                    }
                } else {
                    size_t offset = buffer.starts_with('+') ? 1 : 0;
                    if (all_of(buffer.begin() + offset, buffer.begin() + buffer.size() - 1, sIsDigit)) {
                        std::string copy;
                        std::copy(buffer.begin() + offset, buffer.begin() + buffer.size() - 1, back_inserter(copy));
                        char *end = 0;
                        uint64_t v = strtoull(copy.c_str(), &end, 10);
                        if (copy.c_str() + copy.size() == end) {
                            if (i8trailing) {
                                if (numeric_limits<int8_t>::max() < v) {
                                    return nullptr;
                                } else {
                                    int8_t vv = (int8_t)v;
                                    return make_shared<ByteTag>(*(uint8_t *)&vv);
                                }
                            } else if (i16trailing) {
                                if (numeric_limits<int16_t>::max() < v) {
                                    return nullptr;
                                } else {
                                    return make_shared<ShortTag>((int16_t)v);
                                }
                            } else {
                                if (numeric_limits<int64_t>::max() < v) {
                                    return nullptr;
                                } else {
                                    return make_shared<LongTag>((int64_t)v);
                                }
                            }
                        }
                    }
                }
            } else if (f32trailing || f64trailing) {
                if (buffer.size() == 1) {
                    return make_shared<StringTag>(u8string((char8_t const *)buffer.data(), buffer.size()));
                }
                std::string copy;
                std::copy(buffer.begin(), buffer.begin() + buffer.size() - 1, back_inserter(copy));
                char *end = 0;
                double v = strtod(copy.c_str(), &end);
                if (copy.c_str() + copy.size() == end) {
                    if (f32trailing) {
                        if (v < numeric_limits<float>::lowest() || numeric_limits<float>::max() < v) {
                            return nullptr;
                        } else {
                            return make_shared<FloatTag>((float)v);
                        }
                    } else {
                        return make_shared<DoubleTag>(v);
                    }
                }
            }
            if (buffer.starts_with('-')) {
                if (all_of(buffer.begin() + 1, buffer.end(), sIsDigit)) {
                    std::string copy;
                    std::copy(buffer.begin() + 1, buffer.begin() + buffer.size(), back_inserter(copy));
                    char *end = 0;
                    uint64_t v = strtoull(copy.c_str(), &end, 10);
                    if (copy.c_str() + copy.size() == end) {
                        if (-(int64_t)numeric_limits<int32_t>::lowest() < v) {
                            return nullptr;
                        } else {
                            return make_shared<IntTag>(-(int32_t)v);
                        }
                    }
                }
            }
            {
                size_t offset = buffer.starts_with('+') ? 1 : 0;
                if (all_of(buffer.begin() + offset, buffer.end(), sIsDigit)) {
                    char *end = 0;
                    uint64_t v = strtoull(buffer.c_str() + offset, &end, 10);
                    if (buffer.c_str() + buffer.size() == end) {
                        if (numeric_limits<int32_t>::max() < v) {
                            return nullptr;
                        }
                        return make_shared<IntTag>((int32_t)v);
                    }
                }
            }
            {
                char *end = 0;
                double v = strtod(buffer.c_str(), &end);
                if (buffer.c_str() + buffer.size() == end) {
                    return make_shared<DoubleTag>(v);
                }
            }
            if (all_of(buffer.begin(), buffer.end(), [](char c) { return ('0' <= c && c <= '9') || ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || c == '_' || c == '-' || c == '.' || c == '+'; })) {
                return make_shared<StringTag>(u8string((char8_t const *)buffer.data(), buffer.size()));
            }
            return nullptr;
        };
        static auto const sSkipWhitespaces = [](istream &in) {
            int b = in.get();
            while (!in.eof()) {
                if (!(b == ' ' || b == '\xd' || b == '\xa')) {
                    in.unget();
                    return;
                }
            }
        };
        sSkipWhitespaces(in);
        int b = in.get();
        if (in.eof()) {
            return nullptr;
        }
        if (b == '{') {
            auto ret = make_shared<CompoundTag>();
            while (!in.eof()) {
                auto key = sReadScalar(in);
                if (!key) {
                    return nullptr;
                }
                auto typedKey = dynamic_pointer_cast<StringTag>(key);
                if (!typedKey) {
                    return nullptr;
                }
                sSkipWhitespaces(in);
                b = in.get();
                if (b != ':') {
                    return nullptr;
                }
                sSkipWhitespaces(in);
                auto value = ReadSnbt(in);
                if (!value) {
                    return nullptr;
                }
                ret->set(typedKey->fValue, value);
                sSkipWhitespaces(in);
                b = in.get();
                if (b == '}') {
                    return ret;
                } else if (b == ',') {
                    continue;
                } else if (b == '"' || b == '\'') {
                    in.unget();
                    continue;
                }
            }
            return nullptr;
        } else if (b == '[') {
            sSkipWhitespaces(in);
            b = in.get();
            shared_ptr<ListTag> instance;
            function<void(shared_ptr<Tag>)> append;
            if (b == 'B' || b == 'I' || b == 'L') {
                int const marker = b;
                b = in.get();
                if (b == ';') {
                    if (marker == 'B') {
                        auto v = make_shared<ByteArrayTag>();
                        sSkipWhitespaces(in);
                        while (!in.eof()) {
                            b = in.get();
                            if (b == ']') {
                                return v;
                            } else if (b == ',') {
                                if (v->fValue.empty()) {
                                    return nullptr;
                                }
                                sSkipWhitespaces(in);
                            } else {
                                in.unget();
                            }
                            auto raw = ReadSnbt(in);
                            if (!raw) {
                                return nullptr;
                            }
                            auto value = dynamic_pointer_cast<ByteTag>(raw);
                            if (!value) {
                                return nullptr;
                            }
                            v->push_back(value->fValue);
                            sSkipWhitespaces(in);
                        }
                    } else if (marker == 'I') {
                        auto v = make_shared<IntArrayTag>();
                        sSkipWhitespaces(in);
                        while (!in.eof()) {
                            b = in.get();
                            if (b == ']') {
                                return v;
                            } else if (b == ',') {
                                if (v->fValue.empty()) {
                                    return nullptr;
                                }
                                sSkipWhitespaces(in);
                            } else {
                                in.unget();
                            }
                            auto raw = ReadSnbt(in);
                            if (!raw) {
                                return nullptr;
                            }
                            auto value = dynamic_pointer_cast<IntTag>(raw);
                            if (!value) {
                                return nullptr;
                            }
                            v->push_back(value->fValue);
                            sSkipWhitespaces(in);
                        }
                    } else {
                        auto v = make_shared<LongArrayTag>();
                        sSkipWhitespaces(in);
                        while (!in.eof()) {
                            b = in.get();
                            if (b == ']') {
                                return v;
                            } else if (b == ',') {
                                if (v->fValue.empty()) {
                                    return nullptr;
                                }
                                sSkipWhitespaces(in);
                            } else {
                                in.unget();
                            }
                            auto raw = ReadSnbt(in);
                            if (!raw) {
                                return nullptr;
                            }
                            auto value = dynamic_pointer_cast<LongTag>(raw);
                            if (!value) {
                                return nullptr;
                            }
                            v->push_back(value->fValue);
                            sSkipWhitespaces(in);
                        }
                    }
                    return nullptr;
                } else {
                    in.unget();
                    in.unget();
                    auto v = make_shared<ListTag>(Tag::Type::End);
                    append = [v](shared_ptr<Tag> const &value) {
                        v->push_back(value);
                    };
                    instance = v;
                }
            } else {
                in.unget();
                auto v = make_shared<ListTag>(Tag::Type::End);
                append = [v](shared_ptr<Tag> const &value) {
                    v->push_back(value);
                };
                instance = v;
            }
            // list
            sSkipWhitespaces(in);
            while (!in.eof()) {
                b = in.get();
                if (b == ']') {
                    return instance;
                } else if (b == ',') {
                    if (instance->empty()) {
                        return nullptr;
                    }
                } else {
                    in.unget();
                }
                auto value = ReadSnbt(in);
                if (!value) {
                    return nullptr;
                }
                if (!instance->push_back(value)) {
                    return nullptr;
                }
                sSkipWhitespaces(in);
            }
            return nullptr;
        } else {
            in.unget();
            return sReadScalar(in);
        }
        return nullptr;
    }

public:
    std::map<std::u8string, std::shared_ptr<Tag>> fValue;
};

template<>
inline std::shared_ptr<ByteTag> CompoundTag::get(std::u8string const &name) const {
    return byteTag(name);
}

template<>
inline std::shared_ptr<ShortTag> CompoundTag::get(std::u8string const &name) const {
    return shortTag(name);
}

template<>
inline std::shared_ptr<IntTag> CompoundTag::get(std::u8string const &name) const {
    return intTag(name);
}

template<>
inline std::shared_ptr<LongTag> CompoundTag::get(std::u8string const &name) const {
    return longTag(name);
}

template<>
inline std::shared_ptr<FloatTag> CompoundTag::get(std::u8string const &name) const {
    return floatTag(name);
}

template<>
inline std::shared_ptr<DoubleTag> CompoundTag::get(std::u8string const &name) const {
    return doubleTag(name);
}

template<>
inline std::shared_ptr<ByteArrayTag> CompoundTag::get(std::u8string const &name) const {
    return byteArrayTag(name);
}

template<>
inline std::shared_ptr<StringTag> CompoundTag::get(std::u8string const &name) const {
    return stringTag(name);
}

template<>
inline std::shared_ptr<ListTag> CompoundTag::get(std::u8string const &name) const {
    return listTag(name);
}

template<>
inline std::shared_ptr<CompoundTag> CompoundTag::get(std::u8string const &name) const {
    return compoundTag(name);
}

template<>
inline std::shared_ptr<IntArrayTag> CompoundTag::get(std::u8string const &name) const {
    return intArrayTag(name);
}

template<>
inline std::shared_ptr<LongArrayTag> CompoundTag::get(std::u8string const &name) const {
    return longArrayTag(name);
}

template<>
inline std::optional<int8_t> CompoundTag::value(std::u8string const &name) const {
    return byte(name);
}

template<>
inline std::optional<int16_t> CompoundTag::value(std::u8string const &name) const {
    return int16(name);
}

template<>
inline std::optional<int32_t> CompoundTag::value(std::u8string const &name) const {
    return int32(name);
}

template<>
inline std::optional<int64_t> CompoundTag::value(std::u8string const &name) const {
    return int64(name);
}

template<>
inline std::optional<float> CompoundTag::value(std::u8string const &name) const {
    return float32(name);
}

template<>
inline std::optional<double> CompoundTag::value(std::u8string const &name) const {
    return float64(name);
}

template<>
inline std::optional<std::u8string> CompoundTag::value(std::u8string const &name) const {
    return string(name);
}

template<>
inline std::optional<bool> CompoundTag::value(std::u8string const &name) const {
    return boolean(name);
}

} // namespace mcfile::nbt
