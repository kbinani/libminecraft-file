#pragma once

namespace mcfile {
namespace nbt {

class CompoundTag : public Tag {
public:
    bool readImpl(::mcfile::stream::InputStreamReader &r) override {
        std::map<std::string, std::shared_ptr<Tag>> tmp;
        while (r.pos() < r.length()) {
            uint8_t type;
            if (!r.read(&type)) {
                break;
            }
            if (type == static_cast<uint8_t>(Tag::Type::End)) {
                break;
            }

            std::string name;
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
            tmp.insert(std::make_pair(name, tag));
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

    [[nodiscard]] bool writeAsRoot(mcfile::stream::OutputStreamWriter &w) const {
        if (!w.write(static_cast<uint8_t>(Tag::Type::Compound))) {
            return false;
        }
        if (!w.write(std::string())) {
            return false;
        }
        if (!writeImpl(w)) {
            return false;
        }
        return true;
    }

    [[nodiscard]] [[deprecated]] bool read(::mcfile::stream::InputStreamReader &r) {
        return readImpl(r);
    }

    [[nodiscard]] [[deprecated]] bool write(mcfile::stream::OutputStreamWriter &w) const {
        return writeImpl(w);
    }

    Tag::Type type() const override { return Tag::Type::Compound; }

    Tag const *query(std::string const &path) const {
        // path: /Level/Sections
        if (path.empty()) {
            return EndTag::Instance();
        }
        std::string p = path;
        Tag const *pivot = this;
        while (!p.empty()) {
            if (p[0] == '/') {
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
                auto pos = p.find_first_of('/');
                std::string name;
                if (pos == std::string::npos) {
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
                        index = std::stoi(name);
                        if (index < 0 || list->size() <= index) {
                            return EndTag::Instance();
                        }
                        auto const &child = list->at(index);
                        if (pos == std::string::npos) {
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
                    if (pos == std::string::npos) {
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

    std::shared_ptr<Tag> &operator[](std::string const &name) {
        return fValue[name];
    }

    void insert(std::pair<std::string, std::shared_ptr<Tag>> const &item) {
        fValue.insert(item);
    }

    void insert(std::initializer_list<std::pair<std::string const, std::shared_ptr<Tag>>> l) {
        for (auto const &it : l) {
            fValue.insert(it);
        }
    }

    decltype(auto) find(std::string const &name) const { return fValue.find(name); }
    decltype(auto) begin() const { return fValue.begin(); }
    decltype(auto) end() const { return fValue.end(); }
    size_t size() const { return fValue.size(); }
    bool empty() const { return fValue.empty(); }

    void erase(std::string const &name) { fValue.erase(name); }

    std::shared_ptr<Tag> tag(std::string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end()) {
            return nullptr;
        }
        return found->second;
    }

    std::shared_ptr<StringTag> stringTag(std::string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::String)
            return nullptr;
        return std::dynamic_pointer_cast<StringTag>(found->second);
    }

    std::shared_ptr<ByteTag> byteTag(std::string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::Byte)
            return nullptr;
        return std::dynamic_pointer_cast<ByteTag>(found->second);
    }

    std::shared_ptr<CompoundTag> compoundTag(std::string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::Compound)
            return nullptr;
        return std::dynamic_pointer_cast<CompoundTag>(found->second);
    }

    std::shared_ptr<ListTag> listTag(std::string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::List)
            return nullptr;
        return std::dynamic_pointer_cast<ListTag>(found->second);
    }

    std::shared_ptr<IntTag> intTag(std::string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::Int)
            return nullptr;
        return std::dynamic_pointer_cast<IntTag>(found->second);
    }

    std::shared_ptr<LongTag> longTag(std::string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::Long)
            return nullptr;
        return std::dynamic_pointer_cast<LongTag>(found->second);
    }

    std::shared_ptr<ShortTag> shortTag(std::string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::Short)
            return nullptr;
        return std::dynamic_pointer_cast<ShortTag>(found->second);
    }

    std::shared_ptr<FloatTag> floatTag(std::string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::Float)
            return nullptr;
        return std::dynamic_pointer_cast<FloatTag>(found->second);
    }

    std::shared_ptr<DoubleTag> doubleTag(std::string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::Double)
            return nullptr;
        return std::dynamic_pointer_cast<DoubleTag>(found->second);
    }

    std::shared_ptr<IntArrayTag> intArrayTag(std::string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::IntArray)
            return nullptr;
        return std::dynamic_pointer_cast<IntArrayTag>(found->second);
    }

    std::shared_ptr<ByteArrayTag> byteArrayTag(std::string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::ByteArray)
            return nullptr;
        return std::dynamic_pointer_cast<ByteArrayTag>(found->second);
    }

    std::shared_ptr<LongArrayTag> longArrayTag(std::string const &name) const {
        auto found = fValue.find(name);
        if (found == fValue.end())
            return nullptr;
        if (!found->second)
            return nullptr;
        if (found->second->type() != Tag::Type::LongArray)
            return nullptr;
        return std::dynamic_pointer_cast<LongArrayTag>(found->second);
    }

    std::optional<int8_t> byte(std::string const &name) const {
        auto v = byteTag(name);
        if (v) {
            return v->fValue;
        } else {
            return std::nullopt;
        }
    }

    std::optional<bool> boolean(std::string const &name) const {
        auto v = byteTag(name);
        if (v) {
            return v->fValue != 0;
        } else {
            return std::nullopt;
        }
    }

    std::optional<int16_t> int16(std::string const &name) const {
        auto v = shortTag(name);
        if (v) {
            return v->fValue;
        } else {
            return std::nullopt;
        }
    }

    std::optional<int32_t> int32(std::string const &name) const {
        auto v = intTag(name);
        if (v) {
            return v->fValue;
        } else {
            return std::nullopt;
        }
    }

    std::optional<int64_t> int64(std::string const &name) const {
        auto v = longTag(name);
        if (v) {
            return v->fValue;
        } else {
            return std::nullopt;
        }
    }

    std::optional<std::string> string(std::string const &name) const {
        auto v = stringTag(name);
        if (v) {
            return v->fValue;
        } else {
            return std::nullopt;
        }
    }

    std::optional<float> float32(std::string const &name) const {
        auto v = floatTag(name);
        if (v) {
            return v->fValue;
        } else {
            return std::nullopt;
        }
    }

    std::optional<double> float64(std::string const &name) const {
        auto v = doubleTag(name);
        if (v) {
            return v->fValue;
        } else {
            return std::nullopt;
        }
    }

    int8_t byte(std::string const &name, int8_t fallback) const {
        auto v = byteTag(name);
        return v ? v->fValue : fallback;
    }

    bool boolean(std::string const &name, bool fallback) const {
        auto v = byteTag(name);
        return v ? v->fValue != 0 : fallback;
    }

    int16_t int16(std::string const &name, int16_t fallback) const {
        auto v = shortTag(name);
        return v ? v->fValue : fallback;
    }

    int32_t int32(std::string const &name, int32_t fallback) const {
        auto v = intTag(name);
        return v ? v->fValue : fallback;
    }

    int64_t int64(std::string const &name, int64_t fallback) const {
        auto v = longTag(name);
        return v ? v->fValue : fallback;
    }

    std::string string(std::string const &name, std::string fallback) const {
        auto v = stringTag(name);
        return v ? v->fValue : fallback;
    }

    float float32(std::string const &name, float fallback) const {
        auto v = floatTag(name);
        return v ? v->fValue : fallback;
    }

    double float64(std::string const &name, double fallback) const {
        auto v = doubleTag(name);
        return v ? v->fValue : fallback;
    }

    void set(std::string const &name, std::shared_ptr<Tag> const &value) {
        fValue[name] = value;
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

    static void ReadUntilEos(stream::InputStreamReader &reader, std::function<bool(std::shared_ptr<CompoundTag> const &value)> callback) {
        ReadUntilEosImpl(reader, callback);
    }

    static void ReadUntilEos(stream::InputStreamReader &reader, std::function<void(std::shared_ptr<CompoundTag> const &value)> callback) {
        ReadUntilEosImpl(reader, [callback](std::shared_ptr<CompoundTag> const &value) {
            callback(value);
            return true;
        });
    }

    static void ReadUntilEos(std::string const &data, stream::ReadOption ro, std::function<bool(std::shared_ptr<CompoundTag> const &value)> callback) {
        auto s = std::make_shared<mcfile::stream::ByteStream>(data);
        mcfile::stream::InputStreamReader reader(s, ro);
        ReadUntilEosImpl(reader, callback);
    }

    static void ReadUntilEos(std::string const &data, stream::ReadOption ro, std::function<void(std::shared_ptr<CompoundTag> const &value)> callback) {
        auto s = std::make_shared<mcfile::stream::ByteStream>(data);
        mcfile::stream::InputStreamReader reader(s, ro);
        ReadUntilEosImpl(reader, [callback](std::shared_ptr<CompoundTag> const &value) {
            callback(value);
            return true;
        });
    }

    static std::shared_ptr<CompoundTag> Read(std::filesystem::path path, stream::ReadOption ro) {
        auto s = std::make_shared<mcfile::stream::FileInputStream>(path);
        mcfile::stream::InputStreamReader isr(s, ro);
        return Read(isr);
    }

    static std::shared_ptr<CompoundTag> Read(std::string const &data, stream::ReadOption ro) {
        auto s = std::make_shared<mcfile::stream::ByteStream>(data);
        mcfile::stream::InputStreamReader isr(s, ro);
        return Read(isr);
    }

    static std::shared_ptr<CompoundTag> Read(std::vector<uint8_t> &buffer, stream::ReadOption ro) {
        auto s = std::make_shared<mcfile::stream::ByteStream>(buffer);
        mcfile::stream::InputStreamReader isr(s, ro);
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
        std::string n;
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

    static std::shared_ptr<CompoundTag> ReadCompressed(std::filesystem::path p, stream::ReadOption ro) {
        auto s = std::make_shared<mcfile::stream::FileInputStream>(p);
        return ReadCompressed(*s, ro);
    }

    static std::shared_ptr<CompoundTag> ReadCompressed(std::string const &data, stream::ReadOption ro) {
        std::vector<uint8_t> buffer;
        buffer.reserve(data.size());
        std::copy(data.begin(), data.end(), std::back_inserter(buffer));
        return ReadCompressed(buffer, ro);
    }

    static std::shared_ptr<CompoundTag> ReadCompressed(mcfile::stream::InputStream &stream, stream::ReadOption ro) {
        std::vector<uint8_t> buffer(stream.length());
        if (!stream.read(buffer.data(), buffer.size(), 1)) {
            return nullptr;
        }
        return ReadCompressed(buffer, ro);
    }

    static std::shared_ptr<CompoundTag> ReadCompressed(std::vector<uint8_t> &buffer, stream::ReadOption ro) {
        if (!Compression::decompress(buffer)) {
            return nullptr;
        }
        return Read(buffer, ro);
    }

    static bool WriteCompressed(CompoundTag const &tag, std::filesystem::path file, stream::WriteOption wo) {
        auto o = std::make_shared<stream::FileOutputStream>(file);
        return WriteCompressed(tag, *o, wo);
    }

    static std::optional<std::string> WriteCompressed(CompoundTag const &tag, stream::WriteOption wo) {
        auto s = std::make_shared<stream::ByteStream>();
        if (!WriteCompressed(tag, *s, wo)) {
            return std::nullopt;
        }
        std::string ret;
        s->drain(ret);
        return ret;
    }

    static bool WriteCompressed(CompoundTag const &tag, mcfile::stream::OutputStream &stream, stream::WriteOption wo) {
        auto s = std::make_shared<stream::ByteStream>();
        stream::OutputStreamWriter osr(s, wo);
        if (!tag.writeAsRoot(osr)) {
            return false;
        }
        std::vector<uint8_t> buffer;
        s->drain(buffer);
        if (!Compression::compress(buffer)) {
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
            std::string name;
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

public:
    std::map<std::string, std::shared_ptr<Tag>> fValue;
};

} // namespace nbt
} // namespace mcfile
