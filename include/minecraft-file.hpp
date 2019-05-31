#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <sstream>
#include <limits>

namespace mcfile {

namespace detail {

class Stream {
public:
    Stream() = default;

    Stream(Stream const &) = delete;

    Stream &operator=(Stream const &) = delete;

    virtual ~Stream() {}

    virtual long length() const = 0;

    virtual bool read(void *buffer, size_t size, size_t count) = 0;

    virtual bool seek(long offset) = 0;

    virtual bool valid() const = 0;

    virtual long pos() const = 0;
};


class FileStream : public Stream {
public:
    explicit FileStream(std::string const &filePath)
            : fFile(nullptr), fLoc(0) {
        FILE *fp = fopen(filePath.c_str(), "rb");

        if (!fp) {
            return;
        }
        if (fseek(fp, 0, SEEK_END) != 0) {
            fclose(fp);
            return;
        }
        long length = ftell(fp);
        if (length == -1L) {
            fclose(fp);
            return;
        }
        if (fseek(fp, 0, SEEK_SET) != 0) {
            fclose(fp);
            return;
        }
        this->fFile = fp;
        this->fLength = length;
    }

    ~FileStream() {
        if (fFile) {
            fclose(fFile);
        }
    }

    FileStream(FileStream const &) = delete;

    FileStream &operator=(FileStream const &) = delete;

    bool read(void *buffer, size_t size, size_t count) override {
        if (!fFile) {
            return false;
        }
        fLoc += size * count;
        return fread(buffer, size, count, fFile) == count;
    }

    bool seek(long offset) override {
        if (!fFile) {
            return false;
        }
        fLoc = offset;
        return fseek(fFile, offset, SEEK_SET) == 0;
    }

    bool valid() const override {
        return fFile != nullptr;
    }

    long length() const override { return fLength; }

    long pos() const override { return fLoc; }

private:
    FILE *fFile;
    long fLength;
    long fLoc;
};


class ByteStream : public Stream {
public:
    explicit ByteStream(std::vector<uint8_t> &buffer)
            : fLoc(0) {
        this->fBuffer.swap(buffer);
    }

    ~ByteStream() {}

    ByteStream(ByteStream const &) = delete;

    ByteStream &operator=(ByteStream const &) = delete;

    bool read(void *buf, size_t size, size_t count) override {
        if (fBuffer.size() <= fLoc + size * count) {
            return false;
        }
        std::copy(fBuffer.begin() + fLoc, fBuffer.begin() + fLoc + size * count, (uint8_t *) buf);
        fLoc += size * count;
        return true;
    }

    bool seek(long offset) override {
        if (offset < 0 || fBuffer.size() <= offset) {
            return false;
        }
        fLoc = offset;
        return true;
    }

    long length() const override { return fBuffer.size(); }

    bool valid() const override { return true; }

    long pos() const override { return fLoc; }

private:
    std::vector<uint8_t> fBuffer;
    long fLoc;
};


class StreamReader {
public:
    explicit StreamReader(std::shared_ptr<Stream> stream)
            : fStream(stream) {
    }

    StreamReader(StreamReader const &) = delete;

    StreamReader &operator=(StreamReader const &) = delete;

    bool valid() const {
        if (!fStream) {
            return false;
        }
        return fStream->valid();
    }

    bool seek(int64_t pos) {
        return fStream->seek(pos);
    }

    bool read(uint8_t *v) {
        return fStream->read(v, sizeof(uint8_t), 1);
    }

    bool read(int8_t *v) {
        return fStream->read(v, sizeof(int8_t), 1);
    }

    bool read(int16_t *v) {
        uint16_t t;
        if (!readRaw(&t)) {
            return false;
        }
        t = Int16FromBE(t);
        *v = *(int16_t *) &t;
        return true;
    }

    bool read(uint16_t *v) {
        uint16_t t;
        if (!readRaw(&t)) {
            return false;
        }
        *v = Int16FromBE(t);
        return true;
    }

    bool read(int32_t *v) {
        uint32_t t;
        if (!readRaw(&t)) {
            return false;
        }
        t = Int32FromBE(t);
        *v = *(int32_t *) &t;
        return true;
    }

    bool read(uint32_t *v) {
        uint32_t t;
        if (!readRaw(&t)) {
            return false;
        }
        *v = Int32FromBE(t);
        return true;
    }

    bool read(int64_t *v) {
        uint64_t t;
        if (!readRaw(&t)) {
            return false;
        }
        t = Int64FromBE(t);
        *v = *(int64_t *) &t;
        return true;
    }

    bool read(uint64_t *v) {
        uint64_t t;
        if (!readRaw(&t)) {
            return false;
        }
        *v = Int64FromBE(t);
        return true;
    }

    bool read(std::vector<uint8_t> &buffer) {
        size_t const count = buffer.size();
        return fStream->read(buffer.data(), sizeof(uint8_t), count);
    }

    bool read(std::string &s) {
        uint16_t length;
        if (!read(&length)) {
            return false;
        }
        std::vector<uint8_t> buffer(length);
        if (!read(buffer)) {
            return false;
        }
        buffer.push_back(0);
        std::string tmp((char const *) buffer.data());
        s.swap(tmp);
        return true;
    }

    long length() const {
        if (!fStream) {
            return 0;
        }
        return fStream->length();
    }

    long pos() const {
        if (!fStream) {
            return 0;
        }
        return fStream->pos();
    }

private:
    template<typename T>
    bool readRaw(T *v) {
        return fStream->read(v, sizeof(T), 1);
    }

    static uint64_t SwapInt64(uint64_t v) {
        return ((v & 0x00000000000000ffLL) << 56)
               | ((v & 0x000000000000ff00LL) << 40)
               | ((v & 0x0000000000ff0000LL) << 24)
               | ((v & 0x00000000ff000000LL) << 8)
               | ((v & 0x000000ff00000000LL) >> 8)
               | ((v & 0x0000ff0000000000LL) >> 24)
               | ((v & 0x00ff000000000000LL) >> 40)
               | ((v & 0xff00000000000000LL) >> 56);
    }

    static uint32_t SwapInt32(uint32_t v) {
        uint32_t r;
        r = v << 24;
        r |= (v & 0x0000FF00) << 8;
        r |= (v & 0x00FF0000) >> 8;
        r |= v >> 24;
        return r;
    }

    static uint16_t SwapInt16(uint16_t v) {
        uint16_t r;
        r = v << 8;
        r |= (v & 0xFF00) >> 8;
        return r;
    }

    static uint64_t Int64FromBE(uint64_t v) {
#if __BYTE_ORDER == __ORDER_BIG_ENDIAN__
        return v;
#else
        return SwapInt64(v);
#endif
    }

    static uint32_t Int32FromBE(uint32_t v) {
#if __BYTE_ORDER == __ORDER_BIG_ENDIAN__
        return v;
#else
        return SwapInt32(v);
#endif
    }

    static uint16_t Int16FromBE(uint16_t v) {
#if __BYTE_ORDER == __ORDER_BIG_ENDIAN__
        return v;
#else
        return SwapInt16(v);
#endif
    }

private:
    std::shared_ptr<Stream> fStream;
};


class Compression {
public:
    Compression() = delete;

    static bool compress(std::vector<uint8_t> &inout) {
        z_stream zs;
        char buff[kSegSize];
        std::vector<uint8_t> outData;

        memset(&zs, 0, sizeof(zs));
        if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK) {
            return false;
        }
        zs.next_in = (Bytef *) inout.data();
        zs.avail_in = inout.size();

        int ret;
        do {
            zs.next_out = reinterpret_cast<Bytef *>(buff);
            zs.avail_out = kSegSize;

            ret = deflate(&zs, Z_FINISH);
            outData.insert(outData.end(), buff, buff + zs.total_out);
        } while (ret == Z_OK);

        int r = deflateEnd(&zs);
        if (ret != Z_STREAM_END) {
            return false;
        }
        if (r != Z_OK) {
            return false;
        }

        inout.swap(outData);
        return true;
    }

    static bool decompress(std::vector<uint8_t> &inout) {
        int ret;
        z_stream zs;
        char buff[kSegSize];
        std::vector<uint8_t> outData;
        unsigned long prevOut = 0;

        memset(&zs, 0, sizeof(zs));
        if (inflateInit(&zs) != Z_OK) {
            return false;
        }

        zs.next_in = (Bytef *) inout.data();
        zs.avail_in = inout.size();

        do {
            zs.next_out = reinterpret_cast<Bytef *>(buff);
            zs.avail_out = kSegSize;

            ret = inflate(&zs, 0);
            outData.insert(outData.end(), buff, buff + (zs.total_out - prevOut));
            prevOut = zs.total_out;
        } while (ret == Z_OK);

        inflateEnd(&zs);
        if (ret != Z_STREAM_END) {
            return false;
        }

        inout.swap(outData);
        return true;
    }

private:
    static const unsigned int kSegSize = 16384;
};


class String {
public:
    String() = delete;

    static std::vector<std::string> Split(std::string const &sentence, char delimiter) {
        std::istringstream input(sentence);
        std::vector<std::string> tokens;
        for (std::string line; std::getline(input, line, delimiter);) {
            tokens.push_back(line);
        }
        return tokens;
    }
};

} // namespace detail

namespace nbt {

class Tag;

class TagFactory {
public:
    static inline std::shared_ptr<Tag> makeTag(uint8_t id, std::string const& name);

private:
    template<typename T>
    static std::shared_ptr<T> makeNamedTag(std::string const& name) {
        auto p = std::make_shared<T>();
        p->fName = name;
        return p;
    }
};

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

    void read(detail::StreamReader& reader) {
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
    virtual bool readImpl(detail::StreamReader& reader) = 0;

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


class EndTag : public Tag {
public:
    bool readImpl(detail::StreamReader&) override { return true; }
    uint8_t id() const override { return TAG_End; }

    static EndTag const* instance() {
        static EndTag s;
        return &s;
    }
};

namespace detail {

template< typename T, uint8_t ID>
class ScalarTag : public Tag {
public:
    bool readImpl(::mcfile::detail::StreamReader& r) override {
        T v;
        if (!r.read(&v)) {
            return false;
        }
        fValue = v;
        return true;
    }

    uint8_t id() const override { return ID; }

public:
    T fValue;
};

}

class ByteTag : public detail::ScalarTag<uint8_t, Tag::TAG_Byte> {};
class ShortTag : public detail::ScalarTag<int16_t, Tag::TAG_Short> {};
class IntTag : public detail::ScalarTag<int32_t, Tag::TAG_Int> {};
class LongTag : public detail::ScalarTag<int64_t, Tag::TAG_Long> {};


class FloatTag : public Tag {
public:
    bool readImpl(::mcfile::detail::StreamReader& r) override {
        uint32_t v;
        if (!r.read(&v)) {
            return false;
        }
        fValue = *(float *)&v;
        return true;
    }

    uint8_t id() const override { return Tag::TAG_Float; }

public:
    float fValue;
};


class DoubleTag : public Tag {
public:
    bool readImpl(::mcfile::detail::StreamReader& r) override {
        uint64_t v;
        if (!r.read(&v)) {
            return false;
        }
        fValue = *(double *)&v;
        return true;
    }

    uint8_t id() const override { return Tag::TAG_Double; }

public:
    double fValue;
};

namespace detail {

template<typename T, uint8_t ID>
class VectorTag : public Tag {
public:
    bool readImpl(::mcfile::detail::StreamReader& r) override {
        uint32_t length;
        if (!r.read(&length)) {
            return false;
        }
        fValue.resize(length);
        for (uint32_t i = 0; i < length; i++) {
            if (!r.read(fValue.data() + i)) {
                return false;
            }
        }
        return true;
    }

    uint8_t id() const override { return ID; }

public:
    std::vector<T> fValue;
};

}

class ByteArrayTag : public detail::VectorTag<uint8_t, Tag::TAG_Byte_Array> {};
class IntArrayTag : public detail::VectorTag<int32_t, Tag::TAG_Int_Array> {};
class LongArrayTag : public detail::VectorTag<int64_t, Tag::TAG_Long_Array> {};


class StringTag : public Tag {
public:
    bool readImpl(::mcfile::detail::StreamReader& r) override {
        std::string tmp;
        if (!r.read(tmp)) {
            return false;
        }
        fValue = tmp;
        return true;
    }

    uint8_t id() const override { return Tag::TAG_String; }

public:
    std::string fValue;
};


class ListTag : public Tag {
public:
    bool readImpl(::mcfile::detail::StreamReader& r) override {
        uint8_t type;
        if (!r.read(&type)) {
            return false;
        }
        int32_t size;
        if (!r.read(&size)) {
            return false;
        }
        std::vector<std::shared_ptr<Tag>> tmp;
        for (int32_t i = 0; i < size; i++) {
            auto tag = TagFactory::makeTag(type, "");
            tag->read(r);
            if (!tag->valid()) {
                return false;
            }
            tmp.push_back(tag);
        }
        tmp.swap(fValue);
        return true;
    }

    uint8_t id() const override { return Tag::TAG_List; }

public:
    std::vector<std::shared_ptr<Tag>> fValue;
};


class CompoundTag : public Tag {
public:
    bool readImpl(::mcfile::detail::StreamReader& r) override {
        std::map<std::string, std::shared_ptr<Tag>> tmp;
        while (r.pos() < r.length()) {
            uint8_t type;
            if (!r.read(&type)) {
                break;
            }
            if (type == Tag::TAG_End) {
                break;
            }

            std::string name;
            if (!r.read(name)) {
                return false;
            }

            auto tag = TagFactory::makeTag(type, name);
            tag->read(r);
            if (!tag->valid()) {
                return false;
            }
            tmp.insert(std::make_pair(name, tag));
        }
        fValue.swap(tmp);
        return true;
    }

    uint8_t id() const override { return Tag::TAG_Compound; }

    Tag const* query(std::string const& path) const {
        // path: /Level/Sections
        if (path.empty()) {
            return EndTag::instance();
        }
        std::string p = path;
        CompoundTag const* pivot = this;
        while (!p.empty()) {
            if (p[0] == '/') {
                if (fValue.size() != 1) {
                    return EndTag::instance();
                }
                auto child = fValue.begin()->second;
                if (p.size() == 1) {
                    return child.get();
                }
                if (child->id() != Tag::TAG_Compound) {
                    return EndTag::instance();
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
                auto child = pivot->fValue.find(name);
                if (child == pivot->fValue.end()) {
                    return EndTag::instance();
                }
                if (pos == std::string::npos) {
                    return child->second.get();
                }
                if (child->second->id() != Tag::TAG_Compound) {
                    return EndTag::instance();
                }
                pivot = child->second->asCompound();
                p = p.substr(pos + 1);
            }
        }
        return EndTag::instance();
    }

public:
    std::map<std::string, std::shared_ptr<Tag>> fValue;
};


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


class Coordinate {
private:
    Coordinate() = delete;

public:
    static int ChunkFromBlock(int block) {
        return block >> 4;
    }

    static int RegionFromChunk(int region) {
        return region >> 5;
    }

    static int RegionFromBlock(int block) {
        return block >> 9;
    }
};

namespace blocks {

using BlockId = uint32_t;

enum : BlockId {
    unknown = 0,
};

namespace minecraft {

enum : BlockId {
    stone = 1,
    granite,
    polished_granite,
    diorite,
    polished_diorite,
    andesite,
    polished_andesite,
    grass_block,
    dirt,
    coarse_dirt,
    podzol,
    cobblestone,
    oak_planks,
    spruce_planks,
    birch_planks,
    jungle_planks,
    acacia_planks,
    dark_oak_planks,
    bedrock,
    sand,
    red_sand,
    gravel,
    gold_ore,
    iron_ore,
    coal_ore,
    oak_log,
    spruce_log,
    birch_log,
    jungle_log,
    acacia_log,
    dark_oak_log,
    stripped_oak_log,
    stripped_spruce_log,
    stripped_birch_log,
    stripped_jungle_log,
    stripped_acacia_log,
    stripped_dark_oak_log,
    stripped_oak_wood,
    stripped_spruce_wood,
    stripped_birch_wood,
    stripped_jungle_wood,
    stripped_acacia_wood,
    stripped_dark_oak_wood,
    oak_wood,
    spruce_wood,
    birch_wood,
    jungle_wood,
    acacia_wood,
    dark_oak_wood,
    sponge,
    wet_sponge,
    glass,
    lapis_ore,
    lapis_block,
    sandstone,
    chiseled_sandstone,
    cut_sandstone,
    white_wool,
    orange_wool,
    magenta_wool,
    light_blue_wool,
    yellow_wool,
    lime_wool,
    pink_wool,
    gray_wool,
    light_gray_wool,
    cyan_wool,
    purple_wool,
    blue_wool,
    brown_wool,
    green_wool,
    red_wool,
    black_wool,
    gold_block,
    iron_block,
    oak_slab,
    spruce_slab,
    birch_slab,
    jungle_slab,
    acacia_slab,
    dark_oak_slab,
    stone_slab,
    sandstone_slab,
    petrified_oak_slab,
    cobblestone_slab,
    brick_slab,
    stone_brick_slab,
    nether_brick_slab,
    quartz_slab,
    red_sandstone_slab,
    purpur_slab,
    prismarine_slab,
    prismarine_brick_slab,
    dark_prismarine_slab,
    smooth_quartz,
    smooth_red_sandstone,
    smooth_sandstone,
    smooth_stone,
    bricks,
    bookshelf,
    mossy_cobblestone,
    obsidian,
    purpur_block,
    purpur_pillar,
    purpur_stairs,
    oak_stairs,
    diamond_ore,
    diamond_block,
    cobblestone_stairs,
    redstone_ore,
    ice,
    snow_block,
    clay,
    pumpkin,
    carved_pumpkin,
    netherrack,
    soul_sand,
    glowstone,
    jack_o_lantern,
    stone_bricks,
    mossy_stone_bricks,
    cracked_stone_bricks,
    chiseled_stone_bricks,
    melon,
    brick_stairs,
    stone_brick_stairs,
    mycelium,
    nether_bricks,
    nether_brick_stairs,
    end_stone,
    end_stone_bricks,
    sandstone_stairs,
    emerald_ore,
    emerald_block,
    spruce_stairs,
    birch_stairs,
    jungle_stairs,
    nether_quartz_ore,
    chiseled_quartz_block,
    quartz_block,
    quartz_pillar,
    quartz_stairs,
    white_terracotta,
    orange_terracotta,
    magenta_terracotta,
    light_blue_terracotta,
    yellow_terracotta,
    lime_terracotta,
    pink_terracotta,
    gray_terracotta,
    light_gray_terracotta,
    cyan_terracotta,
    purple_terracotta,
    blue_terracotta,
    brown_terracotta,
    green_terracotta,
    red_terracotta,
    black_terracotta,
    hay_block,
    terracotta,
    coal_block,
    packed_ice,
    acacia_stairs,
    dark_oak_stairs,
    white_stained_glass,
    orange_stained_glass,
    magenta_stained_glass,
    light_blue_stained_glass,
    yellow_stained_glass,
    lime_stained_glass,
    pink_stained_glass,
    gray_stained_glass,
    light_gray_stained_glass,
    cyan_stained_glass,
    purple_stained_glass,
    blue_stained_glass,
    brown_stained_glass,
    green_stained_glass,
    red_stained_glass,
    black_stained_glass,
    prismarine,
    prismarine_bricks,
    dark_prismarine,
    prismarine_stairs,
    prismarine_brick_stairs,
    dark_prismarine_stairs,
    sea_lantern,
    red_sandstone,
    chiseled_red_sandstone,
    cut_red_sandstone,
    red_sandstone_stairs,
    magma_block,
    nether_wart_block,
    red_nether_bricks,
    bone_block,
    white_concrete,
    orange_concrete,
    magenta_concrete,
    light_blue_concrete,
    yellow_concrete,
    lime_concrete,
    pink_concrete,
    gray_concrete,
    light_gray_concrete,
    cyan_concrete,
    purple_concrete,
    blue_concrete,
    brown_concrete,
    green_concrete,
    red_concrete,
    black_concrete,
    white_concrete_powder,
    orange_concrete_powder,
    magenta_concrete_powder,
    light_blue_concrete_powder,
    yellow_concrete_powder,
    lime_concrete_powder,
    pink_concrete_powder,
    gray_concrete_powder,
    light_gray_concrete_powder,
    cyan_concrete_powder,
    purple_concrete_powder,
    blue_concrete_powder,
    brown_concrete_powder,
    green_concrete_powder,
    red_concrete_powder,
    black_concrete_powder,
    dead_tube_coral_block,
    dead_brain_coral_block,
    dead_bubble_coral_block,
    dead_fire_coral_block,
    dead_horn_coral_block,
    tube_coral_block,
    brain_coral_block,
    bubble_coral_block,
    fire_coral_block,
    horn_coral_block,
    blue_ice,
    dried_kelp_block,
    oak_sapling,
    spruce_sapling,
    birch_sapling,
    jungle_sapling,
    acacia_sapling,
    dark_oak_sapling,
    oak_leaves,
    spruce_leaves,
    birch_leaves,
    jungle_leaves,
    acacia_leaves,
    dark_oak_leaves,
    cobweb,
    grass,
    fern,
    dead_bush,
    seagrass,
    sea_pickle,
    dandelion,
    poppy,
    blue_orchid,
    allium,
    azure_bluet,
    red_tulip,
    orange_tulip,
    white_tulip,
    pink_tulip,
    oxeye_daisy,
    brown_mushroom,
    red_mushroom,
    torch,
    end_rod,
    chorus_plant,
    chorus_flower,
    chest,
    crafting_table,
    farmland,
    furnace,
    ladder,
    snow,
    cactus,
    jukebox,
    oak_fence,
    spruce_fence,
    birch_fence,
    jungle_fence,
    acacia_fence,
    dark_oak_fence,
    infested_stone,
    infested_cobblestone,
    infested_stone_bricks,
    infested_mossy_stone_bricks,
    infested_cracked_stone_bricks,
    infested_chiseled_stone_bricks,
    brown_mushroom_block,
    red_mushroom_block,
    mushroom_stem,
    iron_bars,
    glass_pane,
    vine,
    lily_pad,
    nether_brick_fence,
    enchanting_table,
    end_portal_frame,
    ender_chest,
    cobblestone_wall,
    mossy_cobblestone_wall,
    anvil,
    chipped_anvil,
    damaged_anvil,
    white_carpet,
    orange_carpet,
    magenta_carpet,
    light_blue_carpet,
    yellow_carpet,
    lime_carpet,
    pink_carpet,
    gray_carpet,
    light_gray_carpet,
    cyan_carpet,
    purple_carpet,
    blue_carpet,
    brown_carpet,
    green_carpet,
    red_carpet,
    black_carpet,
    slime_block,
    grass_path,
    sunflower,
    lilac,
    rose_bush,
    peony,
    tall_grass,
    large_fern,
    white_stained_glass_pane,
    orange_stained_glass_pane,
    magenta_stained_glass_pane,
    light_blue_stained_glass_pane,
    yellow_stained_glass_pane,
    lime_stained_glass_pane,
    pink_stained_glass_pane,
    gray_stained_glass_pane,
    light_gray_stained_glass_pane,
    cyan_stained_glass_pane,
    purple_stained_glass_pane,
    blue_stained_glass_pane,
    brown_stained_glass_pane,
    green_stained_glass_pane,
    red_stained_glass_pane,
    black_stained_glass_pane,
    shulker_box,
    white_shulker_box,
    orange_shulker_box,
    magenta_shulker_box,
    light_blue_shulker_box,
    yellow_shulker_box,
    lime_shulker_box,
    pink_shulker_box,
    gray_shulker_box,
    light_gray_shulker_box,
    cyan_shulker_box,
    purple_shulker_box,
    blue_shulker_box,
    brown_shulker_box,
    green_shulker_box,
    red_shulker_box,
    black_shulker_box,
    white_glazed_terracotta,
    orange_glazed_terracotta,
    magenta_glazed_terracotta,
    light_blue_glazed_terracotta,
    yellow_glazed_terracotta,
    lime_glazed_terracotta,
    pink_glazed_terracotta,
    gray_glazed_terracotta,
    light_gray_glazed_terracotta,
    cyan_glazed_terracotta,
    purple_glazed_terracotta,
    blue_glazed_terracotta,
    brown_glazed_terracotta,
    green_glazed_terracotta,
    red_glazed_terracotta,
    black_glazed_terracotta,
    tube_coral,
    brain_coral,
    bubble_coral,
    fire_coral,
    horn_coral,
    tube_coral_fan,
    brain_coral_fan,
    bubble_coral_fan,
    fire_coral_fan,
    horn_coral_fan,
    dead_tube_coral_fan,
    dead_brain_coral_fan,
    dead_bubble_coral_fan,
    dead_fire_coral_fan,
    dead_horn_coral_fan,
    painting,
    sign,
    white_bed,
    orange_bed,
    magenta_bed,
    light_blue_bed,
    yellow_bed,
    lime_bed,
    pink_bed,
    gray_bed,
    light_gray_bed,
    cyan_bed,
    purple_bed,
    blue_bed,
    brown_bed,
    green_bed,
    red_bed,
    black_bed,
    item_frame,
    flower_pot,
    skeleton_skull,
    wither_skeleton_skull,
    player_head,
    zombie_head,
    creeper_head,
    dragon_head,
    armor_stand,
    white_banner,
    orange_banner,
    magenta_banner,
    light_blue_banner,
    yellow_banner,
    lime_banner,
    pink_banner,
    gray_banner,
    light_gray_banner,
    cyan_banner,
    purple_banner,
    blue_banner,
    brown_banner,
    green_banner,
    red_banner,
    black_banner,
    end_crystal,
    dispenser,
    note_block,
    sticky_piston,
    piston,
    tnt,
    lever,
    stone_pressure_plate,
    oak_pressure_plate,
    spruce_pressure_plate,
    birch_pressure_plate,
    jungle_pressure_plate,
    acacia_pressure_plate,
    dark_oak_pressure_plate,
    redstone_torch,
    stone_button,
    oak_trapdoor,
    spruce_trapdoor,
    birch_trapdoor,
    jungle_trapdoor,
    acacia_trapdoor,
    dark_oak_trapdoor,
    oak_fence_gate,
    spruce_fence_gate,
    birch_fence_gate,
    jungle_fence_gate,
    acacia_fence_gate,
    dark_oak_fence_gate,
    redstone_lamp,
    tripwire_hook,
    oak_button,
    spruce_button,
    birch_button,
    jungle_button,
    acacia_button,
    dark_oak_button,
    trapped_chest,
    light_weighted_pressure_plate,
    heavy_weighted_pressure_plate,
    daylight_detector,
    redstone_block,
    hopper,
    dropper,
    iron_trapdoor,
    observer,
    iron_door,
    oak_door,
    spruce_door,
    birch_door,
    jungle_door,
    acacia_door,
    dark_oak_door,
    repeater,
    comparator,
    redstone,
    powered_rail,
    detector_rail,
    rail,
    activator_rail,
    minecart,
    saddle,
    oak_boat,
    chest_minecart,
    furnace_minecart,
    carrot_on_a_stick,
    tnt_minecart,
    hopper_minecart,
    elytra,
    spruce_boat,
    birch_boat,
    jungle_boat,
    acacia_boat,
    dark_oak_boat,
    beacon,
    turtle_egg,
    conduit,
    scute,
    coal,
    charcoal,
    diamond,
    iron_ingot,
    gold_ingot,
    stick,
    bowl,
    string,
    feather,
    gunpowder,
    wheat_seeds,
    wheat,
    flint,
    bucket,
    water_bucket,
    lava_bucket,
    snowball,
    leather,
    milk_bucket,
    pufferfish_bucket,
    salmon_bucket,
    cod_bucket,
    tropical_fish_bucket,
    brick,
    clay_ball,
    sugar_cane,
    kelp,
    paper,
    book,
    slime_ball,
    egg,
    glowstone_dust,
    ink_sac,
    rose_red,
    cactus_green,
    cocoa_beans,
    lapis_lazuli,
    purple_dye,
    cyan_dye,
    light_gray_dye,
    gray_dye,
    pink_dye,
    lime_dye,
    dandelion_yellow,
    light_blue_dye,
    magenta_dye,
    orange_dye,
    bone_meal,
    bone,
    sugar,
    pumpkin_seeds,
    melon_seeds,
    ender_pearl,
    blaze_rod,
    gold_nugget,
    nether_wart,
    ender_eye,
    bat_spawn_egg,
    blaze_spawn_egg,
    cave_spider_spawn_egg,
    chicken_spawn_egg,
    cod_spawn_egg,
    cow_spawn_egg,
    creeper_spawn_egg,
    dolphin_spawn_egg,
    donkey_spawn_egg,
    drowned_spawn_egg,
    elder_guardian_spawn_egg,
    enderman_spawn_egg,
    endermite_spawn_egg,
    evoker_spawn_egg,
    ghast_spawn_egg,
    guardian_spawn_egg,
    horse_spawn_egg,
    husk_spawn_egg,
    llama_spawn_egg,
    magma_cube_spawn_egg,
    mooshroom_spawn_egg,
    mule_spawn_egg,
    ocelot_spawn_egg,
    parrot_spawn_egg,
    phantom_spawn_egg,
    pig_spawn_egg,
    polar_bear_spawn_egg,
    pufferfish_spawn_egg,
    rabbit_spawn_egg,
    salmon_spawn_egg,
    sheep_spawn_egg,
    shulker_spawn_egg,
    silverfish_spawn_egg,
    skeleton_spawn_egg,
    skeleton_horse_spawn_egg,
    slime_spawn_egg,
    spider_spawn_egg,
    squid_spawn_egg,
    stray_spawn_egg,
    tropical_fish_spawn_egg,
    turtle_spawn_egg,
    vex_spawn_egg,
    villager_spawn_egg,
    vindicator_spawn_egg,
    witch_spawn_egg,
    wither_skeleton_spawn_egg,
    wolf_spawn_egg,
    zombie_spawn_egg,
    zombie_horse_spawn_egg,
    zombie_pigman_spawn_egg,
    zombie_villager_spawn_egg,
    experience_bottle,
    fire_charge,
    writable_book,
    emerald,
    map,
    nether_star,
    firework_rocket,
    firework_star,
    nether_brick,
    quartz,
    prismarine_shard,
    prismarine_crystals,
    rabbit_hide,
    iron_horse_armor,
    golden_horse_armor,
    diamond_horse_armor,
    chorus_fruit,
    popped_chorus_fruit,
    beetroot_seeds,
    shulker_shell,
    iron_nugget,
    music_disc_13,
    music_disc_cat,
    music_disc_blocks,
    music_disc_chirp,
    music_disc_far,
    music_disc_mall,
    music_disc_mellohi,
    music_disc_stal,
    music_disc_strad,
    music_disc_ward,
    music_disc_11,
    music_disc_wait,
    nautilus_shell,
    heart_of_the_sea,
    apple,
    mushroom_stew,
    bread,
    porkchop,
    cooked_porkchop,
    golden_apple,
    enchanted_golden_apple,
    cod,
    salmon,
    tropical_fish,
    pufferfish,
    cooked_cod,
    cooked_salmon,
    cake,
    cookie,
    melon_slice,
    dried_kelp,
    beef,
    cooked_beef,
    chicken,
    cooked_chicken,
    rotten_flesh,
    spider_eye,
    carrot,
    potato,
    baked_potato,
    poisonous_potato,
    pumpkin_pie,
    rabbit,
    cooked_rabbit,
    rabbit_stew,
    mutton,
    cooked_mutton,
    beetroot,
    beetroot_soup,
    iron_shovel,
    iron_pickaxe,
    iron_axe,
    flint_and_steel,
    wooden_shovel,
    wooden_pickaxe,
    wooden_axe,
    stone_shovel,
    stone_pickaxe,
    stone_axe,
    diamond_shovel,
    diamond_pickaxe,
    diamond_axe,
    golden_shovel,
    golden_pickaxe,
    golden_axe,
    wooden_hoe,
    stone_hoe,
    iron_hoe,
    diamond_hoe,
    golden_hoe,
    compass,
    fishing_rod,
    clock,
    shears,
    enchanted_book,
    lead,
    name_tag,
    turtle_helmet,
    bow,
    arrow,
    iron_sword,
    wooden_sword,
    stone_sword,
    diamond_sword,
    golden_sword,
    leather_helmet,
    leather_chestplate,
    leather_leggings,
    leather_boots,
    chainmail_helmet,
    chainmail_chestplate,
    chainmail_leggings,
    chainmail_boots,
    iron_helmet,
    iron_chestplate,
    iron_leggings,
    iron_boots,
    diamond_helmet,
    diamond_chestplate,
    diamond_leggings,
    diamond_boots,
    golden_helmet,
    golden_chestplate,
    golden_leggings,
    golden_boots,
    spectral_arrow,
    tipped_arrow,
    shield,
    totem_of_undying,
    trident,
    ghast_tear,
    potion,
    glass_bottle,
    fermented_spider_eye,
    blaze_powder,
    magma_cream,
    brewing_stand,
    cauldron,
    glistering_melon_slice,
    golden_carrot,
    rabbit_foot,
    dragon_breath,
    splash_potion,
    lingering_potion,
    phantom_membrane,
    command_block,
    repeating_command_block,
    chain_command_block,
    command_block_minecart,
    debug_stick,
    dragon_egg,
    written_book,
    filled_map,
    knowledge_book,
    spawner,
    barrier,
    structure_block,
    structure_void,
    air,
    cave_air,
    void_air,
    water,
    lava,
    carrots,
    potatoes,
    beetroots,
    melon_stem,
    attached_melon_stem,
    pumpkin_stem,
    attached_pumpkin_stem,
    piston_head,
    moving_piston,
    fire,
    nether_portal,
    end_portal,
    end_gateway,
    redstone_wall_torch,
    redstone_wire,
    tripwire,
    frosted_ice,
    bubble_column,
    tall_seagrass,
    kelp_plant,
    tube_coral_wall_fan,
    brain_coral_wall_fan,
    bubble_coral_wall_fan,
    fire_coral_wall_fan,
    horn_coral_wall_fan,
    dead_tube_coral_wall_fan,
    dead_brain_coral_wall_fan,
    dead_bubble_coral_wall_fan,
    dead_fire_coral_wall_fan,
    dead_horn_coral_wall_fan,
    wall_sign,
    wall_torch,
    white_wall_banner,
    orange_wall_banner,
    magenta_wall_banner,
    light_blue_wall_banner,
    yellow_wall_banner,
    lime_wall_banner,
    pink_wall_banner,
    gray_wall_banner,
    light_gray_wall_banner,
    cyan_wall_banner,
    purple_wall_banner,
    blue_wall_banner,
    brown_wall_banner,
    green_wall_banner,
    red_wall_banner,
    black_wall_banner,
    skeleton_wall_skull,
    wither_skeleton_wall_skull,
    player_wall_head,
    zombie_wall_head,
    creeper_wall_head,
    dragon_wall_head,
    potted_oak_sapling,
    potted_spruce_sapling,
    potted_birch_sapling,
    potted_acacia_sapling,
    potted_jungle_sapling,
    potted_dark_oak_sapling,
    potted_brown_mushroom,
    potted_red_mushroom,
    potted_allium,
    potted_azure_bluet,
    potted_blue_orchid,
    potted_cactus,
    potted_dandelion,
    potted_dead_bush,
    potted_fern,
    potted_oxeye_daisy,
    potted_poppy,
    potted_orange_tulip,
    potted_pink_tulip,
    potted_red_tulip,
    potted_white_tulip,

    minecraft_max_block_id,
};

} // namespace minecraft

static inline BlockId FromName(std::string const& name) {
    static std::map<std::string, BlockId> const mapping = {
        {"minecraft:acacia_boat", minecraft::acacia_boat},
        {"minecraft:acacia_button", minecraft::acacia_button},
        {"minecraft:acacia_door", minecraft::acacia_door},
        {"minecraft:acacia_fence", minecraft::acacia_fence},
        {"minecraft:acacia_fence_gate", minecraft::acacia_fence_gate},
        {"minecraft:acacia_leaves", minecraft::acacia_leaves},
        {"minecraft:acacia_log", minecraft::acacia_log},
        {"minecraft:acacia_planks", minecraft::acacia_planks},
        {"minecraft:acacia_pressure_plate", minecraft::acacia_pressure_plate},
        {"minecraft:acacia_sapling", minecraft::acacia_sapling},
        {"minecraft:acacia_slab", minecraft::acacia_slab},
        {"minecraft:acacia_stairs", minecraft::acacia_stairs},
        {"minecraft:acacia_trapdoor", minecraft::acacia_trapdoor},
        {"minecraft:acacia_wood", minecraft::acacia_wood},
        {"minecraft:activator_rail", minecraft::activator_rail},
        {"minecraft:air", minecraft::air},
        {"minecraft:allium", minecraft::allium},
        {"minecraft:andesite", minecraft::andesite},
        {"minecraft:anvil", minecraft::anvil},
        {"minecraft:apple", minecraft::apple},
        {"minecraft:armor_stand", minecraft::armor_stand},
        {"minecraft:arrow", minecraft::arrow},
        {"minecraft:attached_melon_stem", minecraft::attached_melon_stem},
        {"minecraft:attached_pumpkin_stem", minecraft::attached_pumpkin_stem},
        {"minecraft:azure_bluet", minecraft::azure_bluet},
        {"minecraft:baked_potato", minecraft::baked_potato},
        {"minecraft:barrier", minecraft::barrier},
        {"minecraft:bat_spawn_egg", minecraft::bat_spawn_egg},
        {"minecraft:beacon", minecraft::beacon},
        {"minecraft:bedrock", minecraft::bedrock},
        {"minecraft:beef", minecraft::beef},
        {"minecraft:beetroot", minecraft::beetroot},
        {"minecraft:beetroot_seeds", minecraft::beetroot_seeds},
        {"minecraft:beetroot_soup", minecraft::beetroot_soup},
        {"minecraft:beetroots", minecraft::beetroots},
        {"minecraft:birch_boat", minecraft::birch_boat},
        {"minecraft:birch_button", minecraft::birch_button},
        {"minecraft:birch_door", minecraft::birch_door},
        {"minecraft:birch_fence", minecraft::birch_fence},
        {"minecraft:birch_fence_gate", minecraft::birch_fence_gate},
        {"minecraft:birch_leaves", minecraft::birch_leaves},
        {"minecraft:birch_log", minecraft::birch_log},
        {"minecraft:birch_planks", minecraft::birch_planks},
        {"minecraft:birch_pressure_plate", minecraft::birch_pressure_plate},
        {"minecraft:birch_sapling", minecraft::birch_sapling},
        {"minecraft:birch_slab", minecraft::birch_slab},
        {"minecraft:birch_stairs", minecraft::birch_stairs},
        {"minecraft:birch_trapdoor", minecraft::birch_trapdoor},
        {"minecraft:birch_wood", minecraft::birch_wood},
        {"minecraft:black_banner", minecraft::black_banner},
        {"minecraft:black_bed", minecraft::black_bed},
        {"minecraft:black_carpet", minecraft::black_carpet},
        {"minecraft:black_concrete", minecraft::black_concrete},
        {"minecraft:black_concrete_powder", minecraft::black_concrete_powder},
        {"minecraft:black_glazed_terracotta", minecraft::black_glazed_terracotta},
        {"minecraft:black_shulker_box", minecraft::black_shulker_box},
        {"minecraft:black_stained_glass", minecraft::black_stained_glass},
        {"minecraft:black_stained_glass_pane", minecraft::black_stained_glass_pane},
        {"minecraft:black_terracotta", minecraft::black_terracotta},
        {"minecraft:black_wall_banner", minecraft::black_wall_banner},
        {"minecraft:black_wool", minecraft::black_wool},
        {"minecraft:blaze_powder", minecraft::blaze_powder},
        {"minecraft:blaze_rod", minecraft::blaze_rod},
        {"minecraft:blaze_spawn_egg", minecraft::blaze_spawn_egg},
        {"minecraft:blue_banner", minecraft::blue_banner},
        {"minecraft:blue_bed", minecraft::blue_bed},
        {"minecraft:blue_carpet", minecraft::blue_carpet},
        {"minecraft:blue_concrete", minecraft::blue_concrete},
        {"minecraft:blue_concrete_powder", minecraft::blue_concrete_powder},
        {"minecraft:blue_glazed_terracotta", minecraft::blue_glazed_terracotta},
        {"minecraft:blue_ice", minecraft::blue_ice},
        {"minecraft:blue_orchid", minecraft::blue_orchid},
        {"minecraft:blue_shulker_box", minecraft::blue_shulker_box},
        {"minecraft:blue_stained_glass", minecraft::blue_stained_glass},
        {"minecraft:blue_stained_glass_pane", minecraft::blue_stained_glass_pane},
        {"minecraft:blue_terracotta", minecraft::blue_terracotta},
        {"minecraft:blue_wall_banner", minecraft::blue_wall_banner},
        {"minecraft:blue_wool", minecraft::blue_wool},
        {"minecraft:bone", minecraft::bone},
        {"minecraft:bone_block", minecraft::bone_block},
        {"minecraft:bone_meal", minecraft::bone_meal},
        {"minecraft:book", minecraft::book},
        {"minecraft:bookshelf", minecraft::bookshelf},
        {"minecraft:bow", minecraft::bow},
        {"minecraft:bowl", minecraft::bowl},
        {"minecraft:brain_coral", minecraft::brain_coral},
        {"minecraft:brain_coral_block", minecraft::brain_coral_block},
        {"minecraft:brain_coral_fan", minecraft::brain_coral_fan},
        {"minecraft:brain_coral_wall_fan", minecraft::brain_coral_wall_fan},
        {"minecraft:bread", minecraft::bread},
        {"minecraft:brewing_stand", minecraft::brewing_stand},
        {"minecraft:brick", minecraft::brick},
        {"minecraft:brick_slab", minecraft::brick_slab},
        {"minecraft:brick_stairs", minecraft::brick_stairs},
        {"minecraft:bricks", minecraft::bricks},
        {"minecraft:brown_banner", minecraft::brown_banner},
        {"minecraft:brown_bed", minecraft::brown_bed},
        {"minecraft:brown_carpet", minecraft::brown_carpet},
        {"minecraft:brown_concrete", minecraft::brown_concrete},
        {"minecraft:brown_concrete_powder", minecraft::brown_concrete_powder},
        {"minecraft:brown_glazed_terracotta", minecraft::brown_glazed_terracotta},
        {"minecraft:brown_mushroom", minecraft::brown_mushroom},
        {"minecraft:brown_mushroom_block", minecraft::brown_mushroom_block},
        {"minecraft:brown_shulker_box", minecraft::brown_shulker_box},
        {"minecraft:brown_stained_glass", minecraft::brown_stained_glass},
        {"minecraft:brown_stained_glass_pane", minecraft::brown_stained_glass_pane},
        {"minecraft:brown_terracotta", minecraft::brown_terracotta},
        {"minecraft:brown_wall_banner", minecraft::brown_wall_banner},
        {"minecraft:brown_wool", minecraft::brown_wool},
        {"minecraft:bubble_column", minecraft::bubble_column},
        {"minecraft:bubble_coral", minecraft::bubble_coral},
        {"minecraft:bubble_coral_block", minecraft::bubble_coral_block},
        {"minecraft:bubble_coral_fan", minecraft::bubble_coral_fan},
        {"minecraft:bubble_coral_wall_fan", minecraft::bubble_coral_wall_fan},
        {"minecraft:bucket", minecraft::bucket},
        {"minecraft:cactus", minecraft::cactus},
        {"minecraft:cactus_green", minecraft::cactus_green},
        {"minecraft:cake", minecraft::cake},
        {"minecraft:carrot", minecraft::carrot},
        {"minecraft:carrot_on_a_stick", minecraft::carrot_on_a_stick},
        {"minecraft:carrots", minecraft::carrots},
        {"minecraft:carved_pumpkin", minecraft::carved_pumpkin},
        {"minecraft:cauldron", minecraft::cauldron},
        {"minecraft:cave_air", minecraft::cave_air},
        {"minecraft:cave_spider_spawn_egg", minecraft::cave_spider_spawn_egg},
        {"minecraft:chain_command_block", minecraft::chain_command_block},
        {"minecraft:chainmail_boots", minecraft::chainmail_boots},
        {"minecraft:chainmail_chestplate", minecraft::chainmail_chestplate},
        {"minecraft:chainmail_helmet", minecraft::chainmail_helmet},
        {"minecraft:chainmail_leggings", minecraft::chainmail_leggings},
        {"minecraft:charcoal", minecraft::charcoal},
        {"minecraft:chest", minecraft::chest},
        {"minecraft:chest_minecart", minecraft::chest_minecart},
        {"minecraft:chicken", minecraft::chicken},
        {"minecraft:chicken_spawn_egg", minecraft::chicken_spawn_egg},
        {"minecraft:chipped_anvil", minecraft::chipped_anvil},
        {"minecraft:chiseled_quartz_block", minecraft::chiseled_quartz_block},
        {"minecraft:chiseled_red_sandstone", minecraft::chiseled_red_sandstone},
        {"minecraft:chiseled_sandstone", minecraft::chiseled_sandstone},
        {"minecraft:chiseled_stone_bricks", minecraft::chiseled_stone_bricks},
        {"minecraft:chorus_flower", minecraft::chorus_flower},
        {"minecraft:chorus_fruit", minecraft::chorus_fruit},
        {"minecraft:chorus_plant", minecraft::chorus_plant},
        {"minecraft:clay", minecraft::clay},
        {"minecraft:clay_ball", minecraft::clay_ball},
        {"minecraft:clock", minecraft::clock},
        {"minecraft:coal", minecraft::coal},
        {"minecraft:coal_block", minecraft::coal_block},
        {"minecraft:coal_ore", minecraft::coal_ore},
        {"minecraft:coarse_dirt", minecraft::coarse_dirt},
        {"minecraft:cobblestone", minecraft::cobblestone},
        {"minecraft:cobblestone_slab", minecraft::cobblestone_slab},
        {"minecraft:cobblestone_stairs", minecraft::cobblestone_stairs},
        {"minecraft:cobblestone_wall", minecraft::cobblestone_wall},
        {"minecraft:cobweb", minecraft::cobweb},
        {"minecraft:cocoa_beans", minecraft::cocoa_beans},
        {"minecraft:cod", minecraft::cod},
        {"minecraft:cod_bucket", minecraft::cod_bucket},
        {"minecraft:cod_spawn_egg", minecraft::cod_spawn_egg},
        {"minecraft:command_block", minecraft::command_block},
        {"minecraft:command_block_minecart", minecraft::command_block_minecart},
        {"minecraft:comparator", minecraft::comparator},
        {"minecraft:compass", minecraft::compass},
        {"minecraft:conduit", minecraft::conduit},
        {"minecraft:cooked_beef", minecraft::cooked_beef},
        {"minecraft:cooked_chicken", minecraft::cooked_chicken},
        {"minecraft:cooked_cod", minecraft::cooked_cod},
        {"minecraft:cooked_mutton", minecraft::cooked_mutton},
        {"minecraft:cooked_porkchop", minecraft::cooked_porkchop},
        {"minecraft:cooked_rabbit", minecraft::cooked_rabbit},
        {"minecraft:cooked_salmon", minecraft::cooked_salmon},
        {"minecraft:cookie", minecraft::cookie},
        {"minecraft:cow_spawn_egg", minecraft::cow_spawn_egg},
        {"minecraft:cracked_stone_bricks", minecraft::cracked_stone_bricks},
        {"minecraft:crafting_table", minecraft::crafting_table},
        {"minecraft:creeper_head", minecraft::creeper_head},
        {"minecraft:creeper_spawn_egg", minecraft::creeper_spawn_egg},
        {"minecraft:creeper_wall_head", minecraft::creeper_wall_head},
        {"minecraft:cut_red_sandstone", minecraft::cut_red_sandstone},
        {"minecraft:cut_sandstone", minecraft::cut_sandstone},
        {"minecraft:cyan_banner", minecraft::cyan_banner},
        {"minecraft:cyan_bed", minecraft::cyan_bed},
        {"minecraft:cyan_carpet", minecraft::cyan_carpet},
        {"minecraft:cyan_concrete", minecraft::cyan_concrete},
        {"minecraft:cyan_concrete_powder", minecraft::cyan_concrete_powder},
        {"minecraft:cyan_dye", minecraft::cyan_dye},
        {"minecraft:cyan_glazed_terracotta", minecraft::cyan_glazed_terracotta},
        {"minecraft:cyan_shulker_box", minecraft::cyan_shulker_box},
        {"minecraft:cyan_stained_glass", minecraft::cyan_stained_glass},
        {"minecraft:cyan_stained_glass_pane", minecraft::cyan_stained_glass_pane},
        {"minecraft:cyan_terracotta", minecraft::cyan_terracotta},
        {"minecraft:cyan_wall_banner", minecraft::cyan_wall_banner},
        {"minecraft:cyan_wool", minecraft::cyan_wool},
        {"minecraft:damaged_anvil", minecraft::damaged_anvil},
        {"minecraft:dandelion", minecraft::dandelion},
        {"minecraft:dandelion_yellow", minecraft::dandelion_yellow},
        {"minecraft:dark_oak_boat", minecraft::dark_oak_boat},
        {"minecraft:dark_oak_button", minecraft::dark_oak_button},
        {"minecraft:dark_oak_door", minecraft::dark_oak_door},
        {"minecraft:dark_oak_fence", minecraft::dark_oak_fence},
        {"minecraft:dark_oak_fence_gate", minecraft::dark_oak_fence_gate},
        {"minecraft:dark_oak_leaves", minecraft::dark_oak_leaves},
        {"minecraft:dark_oak_log", minecraft::dark_oak_log},
        {"minecraft:dark_oak_planks", minecraft::dark_oak_planks},
        {"minecraft:dark_oak_pressure_plate", minecraft::dark_oak_pressure_plate},
        {"minecraft:dark_oak_sapling", minecraft::dark_oak_sapling},
        {"minecraft:dark_oak_slab", minecraft::dark_oak_slab},
        {"minecraft:dark_oak_stairs", minecraft::dark_oak_stairs},
        {"minecraft:dark_oak_trapdoor", minecraft::dark_oak_trapdoor},
        {"minecraft:dark_oak_wood", minecraft::dark_oak_wood},
        {"minecraft:dark_prismarine", minecraft::dark_prismarine},
        {"minecraft:dark_prismarine_slab", minecraft::dark_prismarine_slab},
        {"minecraft:dark_prismarine_stairs", minecraft::dark_prismarine_stairs},
        {"minecraft:daylight_detector", minecraft::daylight_detector},
        {"minecraft:dead_brain_coral_block", minecraft::dead_brain_coral_block},
        {"minecraft:dead_brain_coral_fan", minecraft::dead_brain_coral_fan},
        {"minecraft:dead_brain_coral_wall_fan", minecraft::dead_brain_coral_wall_fan},
        {"minecraft:dead_bubble_coral_block", minecraft::dead_bubble_coral_block},
        {"minecraft:dead_bubble_coral_fan", minecraft::dead_bubble_coral_fan},
        {"minecraft:dead_bubble_coral_wall_fan", minecraft::dead_bubble_coral_wall_fan},
        {"minecraft:dead_bush", minecraft::dead_bush},
        {"minecraft:dead_fire_coral_block", minecraft::dead_fire_coral_block},
        {"minecraft:dead_fire_coral_fan", minecraft::dead_fire_coral_fan},
        {"minecraft:dead_fire_coral_wall_fan", minecraft::dead_fire_coral_wall_fan},
        {"minecraft:dead_horn_coral_block", minecraft::dead_horn_coral_block},
        {"minecraft:dead_horn_coral_fan", minecraft::dead_horn_coral_fan},
        {"minecraft:dead_horn_coral_wall_fan", minecraft::dead_horn_coral_wall_fan},
        {"minecraft:dead_tube_coral_block", minecraft::dead_tube_coral_block},
        {"minecraft:dead_tube_coral_fan", minecraft::dead_tube_coral_fan},
        {"minecraft:dead_tube_coral_wall_fan", minecraft::dead_tube_coral_wall_fan},
        {"minecraft:debug_stick", minecraft::debug_stick},
        {"minecraft:detector_rail", minecraft::detector_rail},
        {"minecraft:diamond", minecraft::diamond},
        {"minecraft:diamond_axe", minecraft::diamond_axe},
        {"minecraft:diamond_block", minecraft::diamond_block},
        {"minecraft:diamond_boots", minecraft::diamond_boots},
        {"minecraft:diamond_chestplate", minecraft::diamond_chestplate},
        {"minecraft:diamond_helmet", minecraft::diamond_helmet},
        {"minecraft:diamond_hoe", minecraft::diamond_hoe},
        {"minecraft:diamond_horse_armor", minecraft::diamond_horse_armor},
        {"minecraft:diamond_leggings", minecraft::diamond_leggings},
        {"minecraft:diamond_ore", minecraft::diamond_ore},
        {"minecraft:diamond_pickaxe", minecraft::diamond_pickaxe},
        {"minecraft:diamond_shovel", minecraft::diamond_shovel},
        {"minecraft:diamond_sword", minecraft::diamond_sword},
        {"minecraft:diorite", minecraft::diorite},
        {"minecraft:dirt", minecraft::dirt},
        {"minecraft:dispenser", minecraft::dispenser},
        {"minecraft:dolphin_spawn_egg", minecraft::dolphin_spawn_egg},
        {"minecraft:donkey_spawn_egg", minecraft::donkey_spawn_egg},
        {"minecraft:dragon_breath", minecraft::dragon_breath},
        {"minecraft:dragon_egg", minecraft::dragon_egg},
        {"minecraft:dragon_head", minecraft::dragon_head},
        {"minecraft:dragon_wall_head", minecraft::dragon_wall_head},
        {"minecraft:dried_kelp", minecraft::dried_kelp},
        {"minecraft:dried_kelp_block", minecraft::dried_kelp_block},
        {"minecraft:dropper", minecraft::dropper},
        {"minecraft:drowned_spawn_egg", minecraft::drowned_spawn_egg},
        {"minecraft:egg", minecraft::egg},
        {"minecraft:elder_guardian_spawn_egg", minecraft::elder_guardian_spawn_egg},
        {"minecraft:elytra", minecraft::elytra},
        {"minecraft:emerald", minecraft::emerald},
        {"minecraft:emerald_block", minecraft::emerald_block},
        {"minecraft:emerald_ore", minecraft::emerald_ore},
        {"minecraft:enchanted_book", minecraft::enchanted_book},
        {"minecraft:enchanted_golden_apple", minecraft::enchanted_golden_apple},
        {"minecraft:enchanting_table", minecraft::enchanting_table},
        {"minecraft:end_crystal", minecraft::end_crystal},
        {"minecraft:end_gateway", minecraft::end_gateway},
        {"minecraft:end_portal", minecraft::end_portal},
        {"minecraft:end_portal_frame", minecraft::end_portal_frame},
        {"minecraft:end_rod", minecraft::end_rod},
        {"minecraft:end_stone", minecraft::end_stone},
        {"minecraft:end_stone_bricks", minecraft::end_stone_bricks},
        {"minecraft:ender_chest", minecraft::ender_chest},
        {"minecraft:ender_eye", minecraft::ender_eye},
        {"minecraft:ender_pearl", minecraft::ender_pearl},
        {"minecraft:enderman_spawn_egg", minecraft::enderman_spawn_egg},
        {"minecraft:endermite_spawn_egg", minecraft::endermite_spawn_egg},
        {"minecraft:evoker_spawn_egg", minecraft::evoker_spawn_egg},
        {"minecraft:experience_bottle", minecraft::experience_bottle},
        {"minecraft:farmland", minecraft::farmland},
        {"minecraft:feather", minecraft::feather},
        {"minecraft:fermented_spider_eye", minecraft::fermented_spider_eye},
        {"minecraft:fern", minecraft::fern},
        {"minecraft:filled_map", minecraft::filled_map},
        {"minecraft:fire", minecraft::fire},
        {"minecraft:fire_charge", minecraft::fire_charge},
        {"minecraft:fire_coral", minecraft::fire_coral},
        {"minecraft:fire_coral_block", minecraft::fire_coral_block},
        {"minecraft:fire_coral_fan", minecraft::fire_coral_fan},
        {"minecraft:fire_coral_wall_fan", minecraft::fire_coral_wall_fan},
        {"minecraft:firework_rocket", minecraft::firework_rocket},
        {"minecraft:firework_star", minecraft::firework_star},
        {"minecraft:fishing_rod", minecraft::fishing_rod},
        {"minecraft:flint", minecraft::flint},
        {"minecraft:flint_and_steel", minecraft::flint_and_steel},
        {"minecraft:flower_pot", minecraft::flower_pot},
        {"minecraft:frosted_ice", minecraft::frosted_ice},
        {"minecraft:furnace", minecraft::furnace},
        {"minecraft:furnace_minecart", minecraft::furnace_minecart},
        {"minecraft:ghast_spawn_egg", minecraft::ghast_spawn_egg},
        {"minecraft:ghast_tear", minecraft::ghast_tear},
        {"minecraft:glass", minecraft::glass},
        {"minecraft:glass_bottle", minecraft::glass_bottle},
        {"minecraft:glass_pane", minecraft::glass_pane},
        {"minecraft:glistering_melon_slice", minecraft::glistering_melon_slice},
        {"minecraft:glowstone", minecraft::glowstone},
        {"minecraft:glowstone_dust", minecraft::glowstone_dust},
        {"minecraft:gold_block", minecraft::gold_block},
        {"minecraft:gold_ingot", minecraft::gold_ingot},
        {"minecraft:gold_nugget", minecraft::gold_nugget},
        {"minecraft:gold_ore", minecraft::gold_ore},
        {"minecraft:golden_apple", minecraft::golden_apple},
        {"minecraft:golden_axe", minecraft::golden_axe},
        {"minecraft:golden_boots", minecraft::golden_boots},
        {"minecraft:golden_carrot", minecraft::golden_carrot},
        {"minecraft:golden_chestplate", minecraft::golden_chestplate},
        {"minecraft:golden_helmet", minecraft::golden_helmet},
        {"minecraft:golden_hoe", minecraft::golden_hoe},
        {"minecraft:golden_horse_armor", minecraft::golden_horse_armor},
        {"minecraft:golden_leggings", minecraft::golden_leggings},
        {"minecraft:golden_pickaxe", minecraft::golden_pickaxe},
        {"minecraft:golden_shovel", minecraft::golden_shovel},
        {"minecraft:golden_sword", minecraft::golden_sword},
        {"minecraft:granite", minecraft::granite},
        {"minecraft:grass", minecraft::grass},
        {"minecraft:grass_block", minecraft::grass_block},
        {"minecraft:grass_path", minecraft::grass_path},
        {"minecraft:gravel", minecraft::gravel},
        {"minecraft:gray_banner", minecraft::gray_banner},
        {"minecraft:gray_bed", minecraft::gray_bed},
        {"minecraft:gray_carpet", minecraft::gray_carpet},
        {"minecraft:gray_concrete", minecraft::gray_concrete},
        {"minecraft:gray_concrete_powder", minecraft::gray_concrete_powder},
        {"minecraft:gray_dye", minecraft::gray_dye},
        {"minecraft:gray_glazed_terracotta", minecraft::gray_glazed_terracotta},
        {"minecraft:gray_shulker_box", minecraft::gray_shulker_box},
        {"minecraft:gray_stained_glass", minecraft::gray_stained_glass},
        {"minecraft:gray_stained_glass_pane", minecraft::gray_stained_glass_pane},
        {"minecraft:gray_terracotta", minecraft::gray_terracotta},
        {"minecraft:gray_wall_banner", minecraft::gray_wall_banner},
        {"minecraft:gray_wool", minecraft::gray_wool},
        {"minecraft:green_banner", minecraft::green_banner},
        {"minecraft:green_bed", minecraft::green_bed},
        {"minecraft:green_carpet", minecraft::green_carpet},
        {"minecraft:green_concrete", minecraft::green_concrete},
        {"minecraft:green_concrete_powder", minecraft::green_concrete_powder},
        {"minecraft:green_glazed_terracotta", minecraft::green_glazed_terracotta},
        {"minecraft:green_shulker_box", minecraft::green_shulker_box},
        {"minecraft:green_stained_glass", minecraft::green_stained_glass},
        {"minecraft:green_stained_glass_pane", minecraft::green_stained_glass_pane},
        {"minecraft:green_terracotta", minecraft::green_terracotta},
        {"minecraft:green_wall_banner", minecraft::green_wall_banner},
        {"minecraft:green_wool", minecraft::green_wool},
        {"minecraft:guardian_spawn_egg", minecraft::guardian_spawn_egg},
        {"minecraft:gunpowder", minecraft::gunpowder},
        {"minecraft:hay_block", minecraft::hay_block},
        {"minecraft:heart_of_the_sea", minecraft::heart_of_the_sea},
        {"minecraft:heavy_weighted_pressure_plate", minecraft::heavy_weighted_pressure_plate},
        {"minecraft:hopper", minecraft::hopper},
        {"minecraft:hopper_minecart", minecraft::hopper_minecart},
        {"minecraft:horn_coral", minecraft::horn_coral},
        {"minecraft:horn_coral_block", minecraft::horn_coral_block},
        {"minecraft:horn_coral_fan", minecraft::horn_coral_fan},
        {"minecraft:horn_coral_wall_fan", minecraft::horn_coral_wall_fan},
        {"minecraft:horse_spawn_egg", minecraft::horse_spawn_egg},
        {"minecraft:husk_spawn_egg", minecraft::husk_spawn_egg},
        {"minecraft:ice", minecraft::ice},
        {"minecraft:infested_chiseled_stone_bricks", minecraft::infested_chiseled_stone_bricks},
        {"minecraft:infested_cobblestone", minecraft::infested_cobblestone},
        {"minecraft:infested_cracked_stone_bricks", minecraft::infested_cracked_stone_bricks},
        {"minecraft:infested_mossy_stone_bricks", minecraft::infested_mossy_stone_bricks},
        {"minecraft:infested_stone", minecraft::infested_stone},
        {"minecraft:infested_stone_bricks", minecraft::infested_stone_bricks},
        {"minecraft:ink_sac", minecraft::ink_sac},
        {"minecraft:iron_axe", minecraft::iron_axe},
        {"minecraft:iron_bars", minecraft::iron_bars},
        {"minecraft:iron_block", minecraft::iron_block},
        {"minecraft:iron_boots", minecraft::iron_boots},
        {"minecraft:iron_chestplate", minecraft::iron_chestplate},
        {"minecraft:iron_door", minecraft::iron_door},
        {"minecraft:iron_helmet", minecraft::iron_helmet},
        {"minecraft:iron_hoe", minecraft::iron_hoe},
        {"minecraft:iron_horse_armor", minecraft::iron_horse_armor},
        {"minecraft:iron_ingot", minecraft::iron_ingot},
        {"minecraft:iron_leggings", minecraft::iron_leggings},
        {"minecraft:iron_nugget", minecraft::iron_nugget},
        {"minecraft:iron_ore", minecraft::iron_ore},
        {"minecraft:iron_pickaxe", minecraft::iron_pickaxe},
        {"minecraft:iron_shovel", minecraft::iron_shovel},
        {"minecraft:iron_sword", minecraft::iron_sword},
        {"minecraft:iron_trapdoor", minecraft::iron_trapdoor},
        {"minecraft:item_frame", minecraft::item_frame},
        {"minecraft:jack_o_lantern", minecraft::jack_o_lantern},
        {"minecraft:jukebox", minecraft::jukebox},
        {"minecraft:jungle_boat", minecraft::jungle_boat},
        {"minecraft:jungle_button", minecraft::jungle_button},
        {"minecraft:jungle_door", minecraft::jungle_door},
        {"minecraft:jungle_fence", minecraft::jungle_fence},
        {"minecraft:jungle_fence_gate", minecraft::jungle_fence_gate},
        {"minecraft:jungle_leaves", minecraft::jungle_leaves},
        {"minecraft:jungle_log", minecraft::jungle_log},
        {"minecraft:jungle_planks", minecraft::jungle_planks},
        {"minecraft:jungle_pressure_plate", minecraft::jungle_pressure_plate},
        {"minecraft:jungle_sapling", minecraft::jungle_sapling},
        {"minecraft:jungle_slab", minecraft::jungle_slab},
        {"minecraft:jungle_stairs", minecraft::jungle_stairs},
        {"minecraft:jungle_trapdoor", minecraft::jungle_trapdoor},
        {"minecraft:jungle_wood", minecraft::jungle_wood},
        {"minecraft:kelp", minecraft::kelp},
        {"minecraft:kelp_plant", minecraft::kelp_plant},
        {"minecraft:knowledge_book", minecraft::knowledge_book},
        {"minecraft:ladder", minecraft::ladder},
        {"minecraft:lapis_block", minecraft::lapis_block},
        {"minecraft:lapis_lazuli", minecraft::lapis_lazuli},
        {"minecraft:lapis_ore", minecraft::lapis_ore},
        {"minecraft:large_fern", minecraft::large_fern},
        {"minecraft:lava", minecraft::lava},
        {"minecraft:lava_bucket", minecraft::lava_bucket},
        {"minecraft:lead", minecraft::lead},
        {"minecraft:leather", minecraft::leather},
        {"minecraft:leather_boots", minecraft::leather_boots},
        {"minecraft:leather_chestplate", minecraft::leather_chestplate},
        {"minecraft:leather_helmet", minecraft::leather_helmet},
        {"minecraft:leather_leggings", minecraft::leather_leggings},
        {"minecraft:lever", minecraft::lever},
        {"minecraft:light_blue_banner", minecraft::light_blue_banner},
        {"minecraft:light_blue_bed", minecraft::light_blue_bed},
        {"minecraft:light_blue_carpet", minecraft::light_blue_carpet},
        {"minecraft:light_blue_concrete", minecraft::light_blue_concrete},
        {"minecraft:light_blue_concrete_powder", minecraft::light_blue_concrete_powder},
        {"minecraft:light_blue_dye", minecraft::light_blue_dye},
        {"minecraft:light_blue_glazed_terracotta", minecraft::light_blue_glazed_terracotta},
        {"minecraft:light_blue_shulker_box", minecraft::light_blue_shulker_box},
        {"minecraft:light_blue_stained_glass", minecraft::light_blue_stained_glass},
        {"minecraft:light_blue_stained_glass_pane", minecraft::light_blue_stained_glass_pane},
        {"minecraft:light_blue_terracotta", minecraft::light_blue_terracotta},
        {"minecraft:light_blue_wall_banner", minecraft::light_blue_wall_banner},
        {"minecraft:light_blue_wool", minecraft::light_blue_wool},
        {"minecraft:light_gray_banner", minecraft::light_gray_banner},
        {"minecraft:light_gray_bed", minecraft::light_gray_bed},
        {"minecraft:light_gray_carpet", minecraft::light_gray_carpet},
        {"minecraft:light_gray_concrete", minecraft::light_gray_concrete},
        {"minecraft:light_gray_concrete_powder", minecraft::light_gray_concrete_powder},
        {"minecraft:light_gray_dye", minecraft::light_gray_dye},
        {"minecraft:light_gray_glazed_terracotta", minecraft::light_gray_glazed_terracotta},
        {"minecraft:light_gray_shulker_box", minecraft::light_gray_shulker_box},
        {"minecraft:light_gray_stained_glass", minecraft::light_gray_stained_glass},
        {"minecraft:light_gray_stained_glass_pane", minecraft::light_gray_stained_glass_pane},
        {"minecraft:light_gray_terracotta", minecraft::light_gray_terracotta},
        {"minecraft:light_gray_wall_banner", minecraft::light_gray_wall_banner},
        {"minecraft:light_gray_wool", minecraft::light_gray_wool},
        {"minecraft:light_weighted_pressure_plate", minecraft::light_weighted_pressure_plate},
        {"minecraft:lilac", minecraft::lilac},
        {"minecraft:lily_pad", minecraft::lily_pad},
        {"minecraft:lime_banner", minecraft::lime_banner},
        {"minecraft:lime_bed", minecraft::lime_bed},
        {"minecraft:lime_carpet", minecraft::lime_carpet},
        {"minecraft:lime_concrete", minecraft::lime_concrete},
        {"minecraft:lime_concrete_powder", minecraft::lime_concrete_powder},
        {"minecraft:lime_dye", minecraft::lime_dye},
        {"minecraft:lime_glazed_terracotta", minecraft::lime_glazed_terracotta},
        {"minecraft:lime_shulker_box", minecraft::lime_shulker_box},
        {"minecraft:lime_stained_glass", minecraft::lime_stained_glass},
        {"minecraft:lime_stained_glass_pane", minecraft::lime_stained_glass_pane},
        {"minecraft:lime_terracotta", minecraft::lime_terracotta},
        {"minecraft:lime_wall_banner", minecraft::lime_wall_banner},
        {"minecraft:lime_wool", minecraft::lime_wool},
        {"minecraft:lingering_potion", minecraft::lingering_potion},
        {"minecraft:llama_spawn_egg", minecraft::llama_spawn_egg},
        {"minecraft:magenta_banner", minecraft::magenta_banner},
        {"minecraft:magenta_bed", minecraft::magenta_bed},
        {"minecraft:magenta_carpet", minecraft::magenta_carpet},
        {"minecraft:magenta_concrete", minecraft::magenta_concrete},
        {"minecraft:magenta_concrete_powder", minecraft::magenta_concrete_powder},
        {"minecraft:magenta_dye", minecraft::magenta_dye},
        {"minecraft:magenta_glazed_terracotta", minecraft::magenta_glazed_terracotta},
        {"minecraft:magenta_shulker_box", minecraft::magenta_shulker_box},
        {"minecraft:magenta_stained_glass", minecraft::magenta_stained_glass},
        {"minecraft:magenta_stained_glass_pane", minecraft::magenta_stained_glass_pane},
        {"minecraft:magenta_terracotta", minecraft::magenta_terracotta},
        {"minecraft:magenta_wall_banner", minecraft::magenta_wall_banner},
        {"minecraft:magenta_wool", minecraft::magenta_wool},
        {"minecraft:magma_block", minecraft::magma_block},
        {"minecraft:magma_cream", minecraft::magma_cream},
        {"minecraft:magma_cube_spawn_egg", minecraft::magma_cube_spawn_egg},
        {"minecraft:map", minecraft::map},
        {"minecraft:melon", minecraft::melon},
        {"minecraft:melon_seeds", minecraft::melon_seeds},
        {"minecraft:melon_slice", minecraft::melon_slice},
        {"minecraft:melon_stem", minecraft::melon_stem},
        {"minecraft:milk_bucket", minecraft::milk_bucket},
        {"minecraft:minecart", minecraft::minecart},
        {"minecraft:mooshroom_spawn_egg", minecraft::mooshroom_spawn_egg},
        {"minecraft:mossy_cobblestone", minecraft::mossy_cobblestone},
        {"minecraft:mossy_cobblestone_wall", minecraft::mossy_cobblestone_wall},
        {"minecraft:mossy_stone_bricks", minecraft::mossy_stone_bricks},
        {"minecraft:moving_piston", minecraft::moving_piston},
        {"minecraft:mule_spawn_egg", minecraft::mule_spawn_egg},
        {"minecraft:mushroom_stem", minecraft::mushroom_stem},
        {"minecraft:mushroom_stew", minecraft::mushroom_stew},
        {"minecraft:music_disc_11", minecraft::music_disc_11},
        {"minecraft:music_disc_13", minecraft::music_disc_13},
        {"minecraft:music_disc_blocks", minecraft::music_disc_blocks},
        {"minecraft:music_disc_cat", minecraft::music_disc_cat},
        {"minecraft:music_disc_chirp", minecraft::music_disc_chirp},
        {"minecraft:music_disc_far", minecraft::music_disc_far},
        {"minecraft:music_disc_mall", minecraft::music_disc_mall},
        {"minecraft:music_disc_mellohi", minecraft::music_disc_mellohi},
        {"minecraft:music_disc_stal", minecraft::music_disc_stal},
        {"minecraft:music_disc_strad", minecraft::music_disc_strad},
        {"minecraft:music_disc_wait", minecraft::music_disc_wait},
        {"minecraft:music_disc_ward", minecraft::music_disc_ward},
        {"minecraft:mutton", minecraft::mutton},
        {"minecraft:mycelium", minecraft::mycelium},
        {"minecraft:name_tag", minecraft::name_tag},
        {"minecraft:nautilus_shell", minecraft::nautilus_shell},
        {"minecraft:nether_brick", minecraft::nether_brick},
        {"minecraft:nether_brick_fence", minecraft::nether_brick_fence},
        {"minecraft:nether_brick_slab", minecraft::nether_brick_slab},
        {"minecraft:nether_brick_stairs", minecraft::nether_brick_stairs},
        {"minecraft:nether_bricks", minecraft::nether_bricks},
        {"minecraft:nether_portal", minecraft::nether_portal},
        {"minecraft:nether_quartz_ore", minecraft::nether_quartz_ore},
        {"minecraft:nether_star", minecraft::nether_star},
        {"minecraft:nether_wart", minecraft::nether_wart},
        {"minecraft:nether_wart_block", minecraft::nether_wart_block},
        {"minecraft:netherrack", minecraft::netherrack},
        {"minecraft:note_block", minecraft::note_block},
        {"minecraft:oak_boat", minecraft::oak_boat},
        {"minecraft:oak_button", minecraft::oak_button},
        {"minecraft:oak_door", minecraft::oak_door},
        {"minecraft:oak_fence", minecraft::oak_fence},
        {"minecraft:oak_fence_gate", minecraft::oak_fence_gate},
        {"minecraft:oak_leaves", minecraft::oak_leaves},
        {"minecraft:oak_log", minecraft::oak_log},
        {"minecraft:oak_planks", minecraft::oak_planks},
        {"minecraft:oak_pressure_plate", minecraft::oak_pressure_plate},
        {"minecraft:oak_sapling", minecraft::oak_sapling},
        {"minecraft:oak_slab", minecraft::oak_slab},
        {"minecraft:oak_stairs", minecraft::oak_stairs},
        {"minecraft:oak_trapdoor", minecraft::oak_trapdoor},
        {"minecraft:oak_wood", minecraft::oak_wood},
        {"minecraft:observer", minecraft::observer},
        {"minecraft:obsidian", minecraft::obsidian},
        {"minecraft:ocelot_spawn_egg", minecraft::ocelot_spawn_egg},
        {"minecraft:orange_banner", minecraft::orange_banner},
        {"minecraft:orange_bed", minecraft::orange_bed},
        {"minecraft:orange_carpet", minecraft::orange_carpet},
        {"minecraft:orange_concrete", minecraft::orange_concrete},
        {"minecraft:orange_concrete_powder", minecraft::orange_concrete_powder},
        {"minecraft:orange_dye", minecraft::orange_dye},
        {"minecraft:orange_glazed_terracotta", minecraft::orange_glazed_terracotta},
        {"minecraft:orange_shulker_box", minecraft::orange_shulker_box},
        {"minecraft:orange_stained_glass", minecraft::orange_stained_glass},
        {"minecraft:orange_stained_glass_pane", minecraft::orange_stained_glass_pane},
        {"minecraft:orange_terracotta", minecraft::orange_terracotta},
        {"minecraft:orange_tulip", minecraft::orange_tulip},
        {"minecraft:orange_wall_banner", minecraft::orange_wall_banner},
        {"minecraft:orange_wool", minecraft::orange_wool},
        {"minecraft:oxeye_daisy", minecraft::oxeye_daisy},
        {"minecraft:packed_ice", minecraft::packed_ice},
        {"minecraft:painting", minecraft::painting},
        {"minecraft:paper", minecraft::paper},
        {"minecraft:parrot_spawn_egg", minecraft::parrot_spawn_egg},
        {"minecraft:peony", minecraft::peony},
        {"minecraft:petrified_oak_slab", minecraft::petrified_oak_slab},
        {"minecraft:phantom_membrane", minecraft::phantom_membrane},
        {"minecraft:phantom_spawn_egg", minecraft::phantom_spawn_egg},
        {"minecraft:pig_spawn_egg", minecraft::pig_spawn_egg},
        {"minecraft:pink_banner", minecraft::pink_banner},
        {"minecraft:pink_bed", minecraft::pink_bed},
        {"minecraft:pink_carpet", minecraft::pink_carpet},
        {"minecraft:pink_concrete", minecraft::pink_concrete},
        {"minecraft:pink_concrete_powder", minecraft::pink_concrete_powder},
        {"minecraft:pink_dye", minecraft::pink_dye},
        {"minecraft:pink_glazed_terracotta", minecraft::pink_glazed_terracotta},
        {"minecraft:pink_shulker_box", minecraft::pink_shulker_box},
        {"minecraft:pink_stained_glass", minecraft::pink_stained_glass},
        {"minecraft:pink_stained_glass_pane", minecraft::pink_stained_glass_pane},
        {"minecraft:pink_terracotta", minecraft::pink_terracotta},
        {"minecraft:pink_tulip", minecraft::pink_tulip},
        {"minecraft:pink_wall_banner", minecraft::pink_wall_banner},
        {"minecraft:pink_wool", minecraft::pink_wool},
        {"minecraft:piston", minecraft::piston},
        {"minecraft:piston_head", minecraft::piston_head},
        {"minecraft:player_head", minecraft::player_head},
        {"minecraft:player_wall_head", minecraft::player_wall_head},
        {"minecraft:podzol", minecraft::podzol},
        {"minecraft:poisonous_potato", minecraft::poisonous_potato},
        {"minecraft:polar_bear_spawn_egg", minecraft::polar_bear_spawn_egg},
        {"minecraft:polished_andesite", minecraft::polished_andesite},
        {"minecraft:polished_diorite", minecraft::polished_diorite},
        {"minecraft:polished_granite", minecraft::polished_granite},
        {"minecraft:popped_chorus_fruit", minecraft::popped_chorus_fruit},
        {"minecraft:poppy", minecraft::poppy},
        {"minecraft:porkchop", minecraft::porkchop},
        {"minecraft:potato", minecraft::potato},
        {"minecraft:potatoes", minecraft::potatoes},
        {"minecraft:potion", minecraft::potion},
        {"minecraft:potted_acacia_sapling", minecraft::potted_acacia_sapling},
        {"minecraft:potted_allium", minecraft::potted_allium},
        {"minecraft:potted_azure_bluet", minecraft::potted_azure_bluet},
        {"minecraft:potted_birch_sapling", minecraft::potted_birch_sapling},
        {"minecraft:potted_blue_orchid", minecraft::potted_blue_orchid},
        {"minecraft:potted_brown_mushroom", minecraft::potted_brown_mushroom},
        {"minecraft:potted_cactus", minecraft::potted_cactus},
        {"minecraft:potted_dandelion", minecraft::potted_dandelion},
        {"minecraft:potted_dark_oak_sapling", minecraft::potted_dark_oak_sapling},
        {"minecraft:potted_dead_bush", minecraft::potted_dead_bush},
        {"minecraft:potted_fern", minecraft::potted_fern},
        {"minecraft:potted_jungle_sapling", minecraft::potted_jungle_sapling},
        {"minecraft:potted_oak_sapling", minecraft::potted_oak_sapling},
        {"minecraft:potted_orange_tulip", minecraft::potted_orange_tulip},
        {"minecraft:potted_oxeye_daisy", minecraft::potted_oxeye_daisy},
        {"minecraft:potted_pink_tulip", minecraft::potted_pink_tulip},
        {"minecraft:potted_poppy", minecraft::potted_poppy},
        {"minecraft:potted_red_mushroom", minecraft::potted_red_mushroom},
        {"minecraft:potted_red_tulip", minecraft::potted_red_tulip},
        {"minecraft:potted_spruce_sapling", minecraft::potted_spruce_sapling},
        {"minecraft:potted_white_tulip", minecraft::potted_white_tulip},
        {"minecraft:powered_rail", minecraft::powered_rail},
        {"minecraft:prismarine", minecraft::prismarine},
        {"minecraft:prismarine_brick_slab", minecraft::prismarine_brick_slab},
        {"minecraft:prismarine_brick_stairs", minecraft::prismarine_brick_stairs},
        {"minecraft:prismarine_bricks", minecraft::prismarine_bricks},
        {"minecraft:prismarine_crystals", minecraft::prismarine_crystals},
        {"minecraft:prismarine_shard", minecraft::prismarine_shard},
        {"minecraft:prismarine_slab", minecraft::prismarine_slab},
        {"minecraft:prismarine_stairs", minecraft::prismarine_stairs},
        {"minecraft:pufferfish", minecraft::pufferfish},
        {"minecraft:pufferfish_bucket", minecraft::pufferfish_bucket},
        {"minecraft:pufferfish_spawn_egg", minecraft::pufferfish_spawn_egg},
        {"minecraft:pumpkin", minecraft::pumpkin},
        {"minecraft:pumpkin_pie", minecraft::pumpkin_pie},
        {"minecraft:pumpkin_seeds", minecraft::pumpkin_seeds},
        {"minecraft:pumpkin_stem", minecraft::pumpkin_stem},
        {"minecraft:purple_banner", minecraft::purple_banner},
        {"minecraft:purple_bed", minecraft::purple_bed},
        {"minecraft:purple_carpet", minecraft::purple_carpet},
        {"minecraft:purple_concrete", minecraft::purple_concrete},
        {"minecraft:purple_concrete_powder", minecraft::purple_concrete_powder},
        {"minecraft:purple_dye", minecraft::purple_dye},
        {"minecraft:purple_glazed_terracotta", minecraft::purple_glazed_terracotta},
        {"minecraft:purple_shulker_box", minecraft::purple_shulker_box},
        {"minecraft:purple_stained_glass", minecraft::purple_stained_glass},
        {"minecraft:purple_stained_glass_pane", minecraft::purple_stained_glass_pane},
        {"minecraft:purple_terracotta", minecraft::purple_terracotta},
        {"minecraft:purple_wall_banner", minecraft::purple_wall_banner},
        {"minecraft:purple_wool", minecraft::purple_wool},
        {"minecraft:purpur_block", minecraft::purpur_block},
        {"minecraft:purpur_pillar", minecraft::purpur_pillar},
        {"minecraft:purpur_slab", minecraft::purpur_slab},
        {"minecraft:purpur_stairs", minecraft::purpur_stairs},
        {"minecraft:quartz", minecraft::quartz},
        {"minecraft:quartz_block", minecraft::quartz_block},
        {"minecraft:quartz_pillar", minecraft::quartz_pillar},
        {"minecraft:quartz_slab", minecraft::quartz_slab},
        {"minecraft:quartz_stairs", minecraft::quartz_stairs},
        {"minecraft:rabbit", minecraft::rabbit},
        {"minecraft:rabbit_foot", minecraft::rabbit_foot},
        {"minecraft:rabbit_hide", minecraft::rabbit_hide},
        {"minecraft:rabbit_spawn_egg", minecraft::rabbit_spawn_egg},
        {"minecraft:rabbit_stew", minecraft::rabbit_stew},
        {"minecraft:rail", minecraft::rail},
        {"minecraft:red_banner", minecraft::red_banner},
        {"minecraft:red_bed", minecraft::red_bed},
        {"minecraft:red_carpet", minecraft::red_carpet},
        {"minecraft:red_concrete", minecraft::red_concrete},
        {"minecraft:red_concrete_powder", minecraft::red_concrete_powder},
        {"minecraft:red_glazed_terracotta", minecraft::red_glazed_terracotta},
        {"minecraft:red_mushroom", minecraft::red_mushroom},
        {"minecraft:red_mushroom_block", minecraft::red_mushroom_block},
        {"minecraft:red_nether_bricks", minecraft::red_nether_bricks},
        {"minecraft:red_sand", minecraft::red_sand},
        {"minecraft:red_sandstone", minecraft::red_sandstone},
        {"minecraft:red_sandstone_slab", minecraft::red_sandstone_slab},
        {"minecraft:red_sandstone_stairs", minecraft::red_sandstone_stairs},
        {"minecraft:red_shulker_box", minecraft::red_shulker_box},
        {"minecraft:red_stained_glass", minecraft::red_stained_glass},
        {"minecraft:red_stained_glass_pane", minecraft::red_stained_glass_pane},
        {"minecraft:red_terracotta", minecraft::red_terracotta},
        {"minecraft:red_tulip", minecraft::red_tulip},
        {"minecraft:red_wall_banner", minecraft::red_wall_banner},
        {"minecraft:red_wool", minecraft::red_wool},
        {"minecraft:redstone", minecraft::redstone},
        {"minecraft:redstone_block", minecraft::redstone_block},
        {"minecraft:redstone_lamp", minecraft::redstone_lamp},
        {"minecraft:redstone_ore", minecraft::redstone_ore},
        {"minecraft:redstone_torch", minecraft::redstone_torch},
        {"minecraft:redstone_wall_torch", minecraft::redstone_wall_torch},
        {"minecraft:redstone_wire", minecraft::redstone_wire},
        {"minecraft:repeater", minecraft::repeater},
        {"minecraft:repeating_command_block", minecraft::repeating_command_block},
        {"minecraft:rose_bush", minecraft::rose_bush},
        {"minecraft:rose_red", minecraft::rose_red},
        {"minecraft:rotten_flesh", minecraft::rotten_flesh},
        {"minecraft:saddle", minecraft::saddle},
        {"minecraft:salmon", minecraft::salmon},
        {"minecraft:salmon_bucket", minecraft::salmon_bucket},
        {"minecraft:salmon_spawn_egg", minecraft::salmon_spawn_egg},
        {"minecraft:sand", minecraft::sand},
        {"minecraft:sandstone", minecraft::sandstone},
        {"minecraft:sandstone_slab", minecraft::sandstone_slab},
        {"minecraft:sandstone_stairs", minecraft::sandstone_stairs},
        {"minecraft:scute", minecraft::scute},
        {"minecraft:sea_lantern", minecraft::sea_lantern},
        {"minecraft:sea_pickle", minecraft::sea_pickle},
        {"minecraft:seagrass", minecraft::seagrass},
        {"minecraft:shears", minecraft::shears},
        {"minecraft:sheep_spawn_egg", minecraft::sheep_spawn_egg},
        {"minecraft:shield", minecraft::shield},
        {"minecraft:shulker_box", minecraft::shulker_box},
        {"minecraft:shulker_shell", minecraft::shulker_shell},
        {"minecraft:shulker_spawn_egg", minecraft::shulker_spawn_egg},
        {"minecraft:sign", minecraft::sign},
        {"minecraft:silverfish_spawn_egg", minecraft::silverfish_spawn_egg},
        {"minecraft:skeleton_horse_spawn_egg", minecraft::skeleton_horse_spawn_egg},
        {"minecraft:skeleton_skull", minecraft::skeleton_skull},
        {"minecraft:skeleton_spawn_egg", minecraft::skeleton_spawn_egg},
        {"minecraft:skeleton_wall_skull", minecraft::skeleton_wall_skull},
        {"minecraft:slime_ball", minecraft::slime_ball},
        {"minecraft:slime_block", minecraft::slime_block},
        {"minecraft:slime_spawn_egg", minecraft::slime_spawn_egg},
        {"minecraft:smooth_quartz", minecraft::smooth_quartz},
        {"minecraft:smooth_red_sandstone", minecraft::smooth_red_sandstone},
        {"minecraft:smooth_sandstone", minecraft::smooth_sandstone},
        {"minecraft:smooth_stone", minecraft::smooth_stone},
        {"minecraft:snow", minecraft::snow},
        {"minecraft:snow_block", minecraft::snow_block},
        {"minecraft:snowball", minecraft::snowball},
        {"minecraft:soul_sand", minecraft::soul_sand},
        {"minecraft:spawner", minecraft::spawner},
        {"minecraft:spectral_arrow", minecraft::spectral_arrow},
        {"minecraft:spider_eye", minecraft::spider_eye},
        {"minecraft:spider_spawn_egg", minecraft::spider_spawn_egg},
        {"minecraft:splash_potion", minecraft::splash_potion},
        {"minecraft:sponge", minecraft::sponge},
        {"minecraft:spruce_boat", minecraft::spruce_boat},
        {"minecraft:spruce_button", minecraft::spruce_button},
        {"minecraft:spruce_door", minecraft::spruce_door},
        {"minecraft:spruce_fence", minecraft::spruce_fence},
        {"minecraft:spruce_fence_gate", minecraft::spruce_fence_gate},
        {"minecraft:spruce_leaves", minecraft::spruce_leaves},
        {"minecraft:spruce_log", minecraft::spruce_log},
        {"minecraft:spruce_planks", minecraft::spruce_planks},
        {"minecraft:spruce_pressure_plate", minecraft::spruce_pressure_plate},
        {"minecraft:spruce_sapling", minecraft::spruce_sapling},
        {"minecraft:spruce_slab", minecraft::spruce_slab},
        {"minecraft:spruce_stairs", minecraft::spruce_stairs},
        {"minecraft:spruce_trapdoor", minecraft::spruce_trapdoor},
        {"minecraft:spruce_wood", minecraft::spruce_wood},
        {"minecraft:squid_spawn_egg", minecraft::squid_spawn_egg},
        {"minecraft:stick", minecraft::stick},
        {"minecraft:sticky_piston", minecraft::sticky_piston},
        {"minecraft:stone", minecraft::stone},
        {"minecraft:stone_axe", minecraft::stone_axe},
        {"minecraft:stone_brick_slab", minecraft::stone_brick_slab},
        {"minecraft:stone_brick_stairs", minecraft::stone_brick_stairs},
        {"minecraft:stone_bricks", minecraft::stone_bricks},
        {"minecraft:stone_button", minecraft::stone_button},
        {"minecraft:stone_hoe", minecraft::stone_hoe},
        {"minecraft:stone_pickaxe", minecraft::stone_pickaxe},
        {"minecraft:stone_pressure_plate", minecraft::stone_pressure_plate},
        {"minecraft:stone_shovel", minecraft::stone_shovel},
        {"minecraft:stone_slab", minecraft::stone_slab},
        {"minecraft:stone_sword", minecraft::stone_sword},
        {"minecraft:stray_spawn_egg", minecraft::stray_spawn_egg},
        {"minecraft:string", minecraft::string},
        {"minecraft:stripped_acacia_log", minecraft::stripped_acacia_log},
        {"minecraft:stripped_acacia_wood", minecraft::stripped_acacia_wood},
        {"minecraft:stripped_birch_log", minecraft::stripped_birch_log},
        {"minecraft:stripped_birch_wood", minecraft::stripped_birch_wood},
        {"minecraft:stripped_dark_oak_log", minecraft::stripped_dark_oak_log},
        {"minecraft:stripped_dark_oak_wood", minecraft::stripped_dark_oak_wood},
        {"minecraft:stripped_jungle_log", minecraft::stripped_jungle_log},
        {"minecraft:stripped_jungle_wood", minecraft::stripped_jungle_wood},
        {"minecraft:stripped_oak_log", minecraft::stripped_oak_log},
        {"minecraft:stripped_oak_wood", minecraft::stripped_oak_wood},
        {"minecraft:stripped_spruce_log", minecraft::stripped_spruce_log},
        {"minecraft:stripped_spruce_wood", minecraft::stripped_spruce_wood},
        {"minecraft:structure_block", minecraft::structure_block},
        {"minecraft:structure_void", minecraft::structure_void},
        {"minecraft:sugar", minecraft::sugar},
        {"minecraft:sugar_cane", minecraft::sugar_cane},
        {"minecraft:sunflower", minecraft::sunflower},
        {"minecraft:tall_grass", minecraft::tall_grass},
        {"minecraft:tall_seagrass", minecraft::tall_seagrass},
        {"minecraft:terracotta", minecraft::terracotta},
        {"minecraft:tipped_arrow", minecraft::tipped_arrow},
        {"minecraft:tnt", minecraft::tnt},
        {"minecraft:tnt_minecart", minecraft::tnt_minecart},
        {"minecraft:torch", minecraft::torch},
        {"minecraft:totem_of_undying", minecraft::totem_of_undying},
        {"minecraft:trapped_chest", minecraft::trapped_chest},
        {"minecraft:trident", minecraft::trident},
        {"minecraft:tripwire", minecraft::tripwire},
        {"minecraft:tripwire_hook", minecraft::tripwire_hook},
        {"minecraft:tropical_fish", minecraft::tropical_fish},
        {"minecraft:tropical_fish_bucket", minecraft::tropical_fish_bucket},
        {"minecraft:tropical_fish_spawn_egg", minecraft::tropical_fish_spawn_egg},
        {"minecraft:tube_coral", minecraft::tube_coral},
        {"minecraft:tube_coral_block", minecraft::tube_coral_block},
        {"minecraft:tube_coral_fan", minecraft::tube_coral_fan},
        {"minecraft:tube_coral_wall_fan", minecraft::tube_coral_wall_fan},
        {"minecraft:turtle_egg", minecraft::turtle_egg},
        {"minecraft:turtle_helmet", minecraft::turtle_helmet},
        {"minecraft:turtle_spawn_egg", minecraft::turtle_spawn_egg},
        {"minecraft:vex_spawn_egg", minecraft::vex_spawn_egg},
        {"minecraft:villager_spawn_egg", minecraft::villager_spawn_egg},
        {"minecraft:vindicator_spawn_egg", minecraft::vindicator_spawn_egg},
        {"minecraft:vine", minecraft::vine},
        {"minecraft:void_air", minecraft::void_air},
        {"minecraft:wall_sign", minecraft::wall_sign},
        {"minecraft:wall_torch", minecraft::wall_torch},
        {"minecraft:water", minecraft::water},
        {"minecraft:water_bucket", minecraft::water_bucket},
        {"minecraft:wet_sponge", minecraft::wet_sponge},
        {"minecraft:wheat", minecraft::wheat},
        {"minecraft:wheat_seeds", minecraft::wheat_seeds},
        {"minecraft:white_banner", minecraft::white_banner},
        {"minecraft:white_bed", minecraft::white_bed},
        {"minecraft:white_carpet", minecraft::white_carpet},
        {"minecraft:white_concrete", minecraft::white_concrete},
        {"minecraft:white_concrete_powder", minecraft::white_concrete_powder},
        {"minecraft:white_glazed_terracotta", minecraft::white_glazed_terracotta},
        {"minecraft:white_shulker_box", minecraft::white_shulker_box},
        {"minecraft:white_stained_glass", minecraft::white_stained_glass},
        {"minecraft:white_stained_glass_pane", minecraft::white_stained_glass_pane},
        {"minecraft:white_terracotta", minecraft::white_terracotta},
        {"minecraft:white_tulip", minecraft::white_tulip},
        {"minecraft:white_wall_banner", minecraft::white_wall_banner},
        {"minecraft:white_wool", minecraft::white_wool},
        {"minecraft:witch_spawn_egg", minecraft::witch_spawn_egg},
        {"minecraft:wither_skeleton_skull", minecraft::wither_skeleton_skull},
        {"minecraft:wither_skeleton_spawn_egg", minecraft::wither_skeleton_spawn_egg},
        {"minecraft:wither_skeleton_wall_skull", minecraft::wither_skeleton_wall_skull},
        {"minecraft:wolf_spawn_egg", minecraft::wolf_spawn_egg},
        {"minecraft:wooden_axe", minecraft::wooden_axe},
        {"minecraft:wooden_hoe", minecraft::wooden_hoe},
        {"minecraft:wooden_pickaxe", minecraft::wooden_pickaxe},
        {"minecraft:wooden_shovel", minecraft::wooden_shovel},
        {"minecraft:wooden_sword", minecraft::wooden_sword},
        {"minecraft:writable_book", minecraft::writable_book},
        {"minecraft:written_book", minecraft::written_book},
        {"minecraft:yellow_banner", minecraft::yellow_banner},
        {"minecraft:yellow_bed", minecraft::yellow_bed},
        {"minecraft:yellow_carpet", minecraft::yellow_carpet},
        {"minecraft:yellow_concrete", minecraft::yellow_concrete},
        {"minecraft:yellow_concrete_powder", minecraft::yellow_concrete_powder},
        {"minecraft:yellow_glazed_terracotta", minecraft::yellow_glazed_terracotta},
        {"minecraft:yellow_shulker_box", minecraft::yellow_shulker_box},
        {"minecraft:yellow_stained_glass", minecraft::yellow_stained_glass},
        {"minecraft:yellow_stained_glass_pane", minecraft::yellow_stained_glass_pane},
        {"minecraft:yellow_terracotta", minecraft::yellow_terracotta},
        {"minecraft:yellow_wall_banner", minecraft::yellow_wall_banner},
        {"minecraft:yellow_wool", minecraft::yellow_wool},
        {"minecraft:zombie_head", minecraft::zombie_head},
        {"minecraft:zombie_horse_spawn_egg", minecraft::zombie_horse_spawn_egg},
        {"minecraft:zombie_pigman_spawn_egg", minecraft::zombie_pigman_spawn_egg},
        {"minecraft:zombie_spawn_egg", minecraft::zombie_spawn_egg},
        {"minecraft:zombie_villager_spawn_egg", minecraft::zombie_villager_spawn_egg},
        {"minecraft:zombie_wall_head", minecraft::zombie_wall_head},
    };
    auto mappingIt = mapping.find(name);
    if (mappingIt == mapping.end()) {
        return unknown;
    }
    return mappingIt->second;
}

} // namespace blocks

class Block {
public:
    Block(std::string const& name, std::map<std::string, std::string> const& properties)
        : fName(name)
        , fProperties(properties)
    {
    }

public:
    std::string const fName;
    std::map<std::string, std::string> const fProperties;
};


class ChunkSection {
public:
    std::shared_ptr<Block> blockAt(int offsetX, int offsetY, int offsetZ) const {
        int const index = paletteIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return nullptr;
        }
        return fPalette[index];
    }

    blocks::BlockId blockIdAt(int offsetX, int offsetY, int offsetZ) const {
        int const index = paletteIndex(offsetX, offsetY, offsetZ);
        if (index < 0) {
            return blocks::unknown;
        }
        return fBlockIdPalette[index];
    }

    static std::shared_ptr<ChunkSection> MakeChunkSection(nbt::CompoundTag const* section) {
        auto yTag = section->query("Y")->asByte();
        if (!yTag) {
            return nullptr;
        }

        auto paletteTag = section->query("Palette")->asList();
        if (!paletteTag) {
            return nullptr;
        }
        std::vector<std::shared_ptr<Block>> palette;
        for (auto entry : paletteTag->fValue) {
            auto tag = entry->asCompound();
            if (!tag) {
                return nullptr;
            }
            auto nameTag = tag->query("Name")->asString();
            if (!nameTag) {
                return nullptr;
            }
            std::map<std::string, std::string> properties;
            auto propertiesTag = tag->query("Properties")->asCompound();
            if (propertiesTag) {
                for (auto p : propertiesTag->fValue) {
                    std::string n = p.first;
                    if (n.empty()) {
                        continue;
                    }
                    auto v = p.second->asString();
                    if (!v) {
                        continue;
                    }
                    properties.insert(std::make_pair(n, v->fValue));
                }
            }
            palette.push_back(std::make_shared<Block>(nameTag->fValue, properties));
        }

        std::vector<blocks::BlockId> blockIdPalette(palette.size());
        for (int i = 0; i < palette.size(); i++) {
            auto block = palette[i];
            blockIdPalette[i] = blocks::FromName(block->fName);
        }

        auto blockStatesTag = section->query("BlockStates")->asLongArray();
        if (!blockStatesTag) {
            return nullptr;
        }

        return std::shared_ptr<ChunkSection>(new ChunkSection((int)yTag->fValue,
                                                              palette,
                                                              blockIdPalette,
                                                              blockStatesTag->fValue));
    }

private:
    ChunkSection(int y, std::vector<std::shared_ptr<Block>> const& palette, std::vector<blocks::BlockId> const& blockIdPalette, std::vector<int64_t> const& blockStates)
        : fY(y)
        , fPalette(palette)
        , fBlockIdPalette(blockIdPalette)
        , fBlockStates(blockStates)
    {
    }

    int paletteIndex(int offsetX, int offsetY, int offsetZ) const {
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return -1;
        }
        size_t const numBits = fBlockStates.size() * 64;
        if (numBits % 4096 != 0) {
            return -1;
        }
        size_t const bitsPerIndex = numBits >> 12;
        int64_t const mask = std::numeric_limits<uint64_t>::max() >> (64 - bitsPerIndex);

        size_t const index = (size_t)offsetY * 16 * 16 + (size_t)offsetZ * 16 + (size_t)offsetX;
        size_t const bitIndex = index * bitsPerIndex;
        size_t const uint64Index = bitIndex >> 6;

        if (fBlockStates.size() <= uint64Index) {
            return -1;
        }
        uint64_t const v0 = *(uint64_t *)(fBlockStates.data() + uint64Index);
        uint64_t const v1 = (uint64Index + 1) < fBlockStates.size() ? *(uint64_t *)(fBlockStates.data() + uint64Index + 1) : 0;

        int const v0Offset = bitIndex - uint64Index * 64;
        int const v0Bits = std::min((uint64Index + 1) * 64 - bitIndex, bitsPerIndex);

        uint64_t const paletteIndex = ((v0 >> v0Offset) & mask) | ((v1 << v0Bits) & mask);

        /*
                   uint64Index                         uint64Index + 1
        |-----------------------------------||-----------------------------------|
        |63                                0||63                                0|
        |-------------------^====^----------||-----------------------------------|
                                 |   <--   0
                                 v0Offset


                   uint64Index                         uint64Index + 1
        |-----------------------------------||-----------------------------------|
        |63                                0||63                                0|
        |===^-------------------------------||---------------------------------^=|
            |    <--        <--       <--   0                                    0
            v0Offset
        */

        if (fPalette.size() <= paletteIndex) {
            return -1;
        }

        return (int)paletteIndex;
    }

public:
    int const fY;
    std::vector<std::shared_ptr<Block>> const fPalette;
    std::vector<blocks::BlockId> const fBlockIdPalette;
    std::vector<int64_t> const fBlockStates;
};


class Chunk {
public:
    std::shared_ptr<Block> blockAt(int x, int y, int z) const {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return nullptr;
        }
        if (y < 0 || 256 <= y) {
            return nullptr;
        }
        int const sectionY = y / 16;
        auto const& section = fSections[sectionY];
        if (!section) {
            return nullptr;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        int const offsetY = y - sectionY * 16;
        return section->blockAt(offsetX, offsetY, offsetZ);
    }

    blocks::BlockId blockIdAt(int x, int y, int z) const {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return blocks::unknown;
        }
        if (y < 0 || 256 <= y) {
            return blocks::unknown;
        }
        int const sectionY = y / 16;
        auto const& section = fSections[sectionY];
        if (!section) {
            return blocks::unknown;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        int const offsetY = y - sectionY * 16;
        return section->blockIdAt(offsetX, offsetY, offsetZ);
    }

    int minBlockX() const { return fChunkX * 16; }
    int maxBlockX() const { return fChunkX * 16 + 15; }

    int minBlockZ() const { return fChunkZ * 16; }
    int maxBlockZ() const { return fChunkZ * 16 + 15; }

    static std::shared_ptr<Chunk> MakeChunk(int chunkX, int chunkZ, std::shared_ptr<nbt::CompoundTag> const& root) {
        auto sectionsTag = root->query("/Level/Sections")->asList();
        if (!sectionsTag) {
            return nullptr;
        }
        std::vector<std::shared_ptr<ChunkSection>> sections;
        for (auto sectionTag : sectionsTag->fValue) {
            auto p = sectionTag->asCompound();
            if (!p) {
                return nullptr;
            }
            sections.push_back(ChunkSection::MakeChunkSection(p));
        }
        return std::shared_ptr<Chunk>(new Chunk(chunkX, chunkZ, sections));
    }

private:
    explicit Chunk(int chunkX, int chunkZ, std::vector<std::shared_ptr<ChunkSection>> const& sections)
        : fChunkX(chunkX)
        , fChunkZ(chunkZ)
    {
        for (auto section : sections) {
            int const y = section->fY;
            if (0 <= y && y < 16) {
                fSections[y] = section;
            }
        }
    }

public:
    int const fChunkX;
    int const fChunkZ;
    std::shared_ptr<ChunkSection> fSections[16];
};

namespace detail {

class ChunkDataSource {
public:
    ChunkDataSource(int chunkX, int chunkZ, uint32_t timestamp, long offset, long length)
            : fChunkX(chunkX), fChunkZ(chunkZ), fTimestamp(timestamp), fOffset(offset), fLength(length) {
    }

    bool load(StreamReader &reader, std::function<void(Chunk const &chunk)> callback) const {
        if (!reader.valid()) {
            return false;
        }
        if (!reader.seek(fOffset + sizeof(uint32_t))) {
            return false;
        }
        uint8_t compressionType;
        if (!reader.read(&compressionType)) {
            return false;
        }
        if (compressionType != 2) {
            return false;
        }
        std::vector<uint8_t> buffer(fLength - 1);
        if (!reader.read(buffer)) {
            return false;
        }
        if (!Compression::decompress(buffer)) {
            return false;
        }
        auto root = std::make_shared<nbt::CompoundTag>();
        auto bs = std::make_shared<ByteStream>(buffer);
        auto sr = std::make_shared<StreamReader>(bs);
        root->read(*sr);
        if (!root->valid()) {
            return false;
        }
        auto chunk = Chunk::MakeChunk(fChunkX, fChunkZ, root);
        if (!chunk) {
            return false;
        }
        callback(*chunk);
        return true;
    }

public:
    int const fChunkX;
    int const fChunkZ;
    uint32_t const fTimestamp;
    long const fOffset;
    long const fLength;
};

} // namespace detail


class Region {
public:
    Region(Region const&) = delete;
    Region& operator = (Region const&) = delete;

    using LoadChunkCallback = std::function<bool(Chunk const&)>;

    bool loadAllChunks(LoadChunkCallback callback) {
        auto fs = std::make_shared<detail::FileStream>(fFilePath);
        detail::StreamReader sr(fs);
        for (int z = 0; z < 32; z++) {
            for (int x = 0; x < 32; x++) {
                if (loadChunkImpl(x, z, callback)) {
                    return false;
                }
            }
        }
        return true;
    }
    
    bool loadChunk(int regionX, int regionZ, LoadChunkCallback callback) {
        if (regionX < 0 || 32 <= regionX || regionZ < 0 || 32 <= regionZ) {
            return false;
        }
        auto fs = std::make_shared<detail::FileStream>(fFilePath);
        detail::StreamReader sr(fs);
        return loadChunkImpl(regionX, regionZ, callback);
    }

    static std::shared_ptr<Region> MakeRegion(std::string const& filePath, int x, int z) {
        return std::shared_ptr<Region>(new Region(filePath, x, z));
    }

    static std::shared_ptr<Region> MakeRegion(std::string const& filePath) {
        // ../directory/r.5.13.mca

        auto basename = filePath;

        auto pos = filePath.find_last_of('/');
        if (pos == std::string::npos) {
            pos = filePath.find_last_of('\\');
        }
        if (pos != std::string::npos) {
            basename = filePath.substr(pos + 1);
        }

        std::vector<std::string> tokens = detail::String::Split(basename, '.');
        if (tokens.size() != 4) {
            return nullptr;
        }
        if (tokens[0] != "r" || tokens[3] != "mca") {
            return nullptr;
        }

        int x, z;
        try {
            x = std::stoi(tokens[1]);
            z = std::stoi(tokens[2]);
        } catch (...) {
            return nullptr;
        }

        return std::shared_ptr<Region>(new Region(filePath, x, z));
    }

    int minBlockX() const { return fX * 32 * 16; }
    int maxBlockX() const { return (fX + 1) * 32 * 16 - 1; }
    int minBlockZ() const { return fZ * 32 * 16; }
    int maxBlockZ() const { return (fZ + 1) * 32 * 16 - 1; }

private:
    Region(std::string const& filePath, int x, int z)
        : fX(x)
        , fZ(z)
        , fFilePath(filePath)
    {
    }

    bool loadChunkImpl(int regionX, int regionZ, LoadChunkCallback callback) {
        auto fs = std::make_shared<detail::FileStream>(fFilePath);
        detail::StreamReader sr(fs);
        int const index = (regionX & 31) + (regionZ & 31) * 32;
        if (!sr.valid()) {
            return false;
        }
        if (!sr.seek(4 * index)) {
            return false;
        }

        uint32_t loc;
        if (!sr.read(&loc)) {
            return false;
        }

        long sectorOffset = loc >> 8;
        if (!sr.seek(kSectorSize + 4 * index)) {
            return false;
        }

        uint32_t timestamp;
        if (!sr.read(&timestamp)) {
            return false;
        }

        if (!sr.seek(sectorOffset * kSectorSize)) {
            return false;
        }
        uint32_t chunkSize;
        if (!sr.read(&chunkSize)) {
            return false;
        }

        int const chunkX = this->fX * 32 + regionX;
        int const chunkZ = this->fZ * 32 + regionZ;
        detail::ChunkDataSource data(chunkX, chunkZ, timestamp, sectorOffset * kSectorSize, chunkSize);
        if (data.load(sr, callback)) {
            return false;
        }

        return true;
    }

public:
    int const fX;
    int const fZ;

private:
    std::string const fFilePath;

    static long const kSectorSize = 4096;
};

class World {
public:
    explicit World(std::string const& rootDirectory)
        : fRootDirectory(rootDirectory)
    {
    }

    std::shared_ptr<Region> region(int regionX, int regionZ) const {
        std::ostringstream ss;
        ss << fRootDirectory << "/region/r." << regionX << "." << regionZ << ".mca";
        auto fileName = ss.str();
        return Region::MakeRegion(fileName);
    }

    bool eachBlock(int minX, int minZ, int maxX, int maxZ, std::function<bool(int x, int y, int z, std::shared_ptr<Block>)> callback) {
        if (minX > maxX || minZ > maxZ) {
            return false;
        }
        int const minRegionX = Coordinate::RegionFromBlock(minX);
        int const maxRegionX = Coordinate::RegionFromBlock(maxX);
        int const minRegionZ = Coordinate::RegionFromBlock(minZ);
        int const maxRegionZ = Coordinate::RegionFromBlock(maxZ);
        for (int regionZ = minRegionZ; regionZ <= maxRegionZ; regionZ++) {
            for (int regionX = minRegionX; regionX <= maxRegionX; regionX++) {
                auto region = this->region(regionX, regionZ);
                if (!region) {
                    continue;
                }
                return region->loadAllChunks([=](Chunk const& chunk) {
                    for (int y = 0; y < 256; y++) {
                        for (int z = std::max(minZ, chunk.minBlockZ()); z <= std::min(maxZ, chunk.maxBlockZ()); z++) {
                            for (int x = std::max(minX, chunk.minBlockX()); x <= std::min(maxX, chunk.maxBlockX()); x++) {
                                auto block = chunk.blockAt(x, y, z);
                                if (!callback(x, y, z, block)) {
                                    return false;
                                }
                            }
                        }
                    }
                    return true;
                });
            }
        }
        return true;
    }

public:
    std::string const fRootDirectory;
};

} // namespace mcfile
