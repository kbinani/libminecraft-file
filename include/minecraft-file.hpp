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

class Stream {
public:
    Stream() = default;
    Stream(Stream const&) = delete;
    Stream& operator = (Stream const&) = delete;
    virtual ~Stream() {}
    virtual long length() const = 0;
    virtual bool read(void *buffer, size_t size, size_t count) = 0;
    virtual bool seek(long offset) = 0;
    virtual bool valid() const = 0;
    virtual long pos() const = 0;
};


class FileStream : public Stream {
public:
    explicit FileStream(std::string const& filePath)
        : fFile(nullptr)
        , fLoc(0)
    {
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

    FileStream(FileStream const&) = delete;
    FileStream& operator = (FileStream const&) = delete;

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
    explicit ByteStream(std::vector<uint8_t>& buffer)
        : fLoc(0)
    {
        this->fBuffer.swap(buffer);
    }

    ~ByteStream() {}

    ByteStream(ByteStream const&) = delete;
    ByteStream& operator = (ByteStream const&) = delete;

    bool read(void *buf, size_t size, size_t count) override {
        if (fBuffer.size() <= fLoc + size * count) {
            return false;
        }
        std::copy(fBuffer.begin() + fLoc, fBuffer.begin() + fLoc + size * count, (uint8_t *)buf);
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
        : fStream(stream)
    {
    }

    StreamReader(StreamReader const&) = delete;
    StreamReader& operator = (StreamReader const&) = delete;

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
        *v = *(int16_t *)&t;
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
        *v = *(int32_t *)&t;
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
        *v = *(int64_t *)&t;
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

    bool read(std::vector<uint8_t>& buffer) {
        size_t const count = buffer.size();
        return fStream->read(buffer.data(), sizeof(uint8_t), count);
    }

    bool read(std::string& s) {
        uint16_t length;
        if (!read(&length)) {
            return false;
        }
        std::vector<uint8_t> buffer(length);
        if (!read(buffer)) {
            return false;
        }
        buffer.push_back(0);
        std::string tmp((char const*)buffer.data());
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
            | ((v & 0x00000000ff000000LL) <<  8)
            | ((v & 0x000000ff00000000LL) >>  8)
            | ((v & 0x0000ff0000000000LL) >> 24)
            | ((v & 0x00ff000000000000LL) >> 40)
            | ((v & 0xff00000000000000LL) >> 56);
    }

    static uint32_t SwapInt32(uint32_t v) {
        uint32_t r;
        r  = v                << 24;
        r |= (v & 0x0000FF00) <<  8;
        r |= (v & 0x00FF0000) >>  8;
        r |= v                >> 24;
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

    static bool compress(std::vector<uint8_t>& inout) {
        z_stream zs;
        char buff[kSegSize];
        std::vector<uint8_t> outData;

        memset(&zs, 0, sizeof(zs));
        if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK) {
            return false;
        }
        zs.next_in = (Bytef *)inout.data();
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

    static bool decompress(std::vector<uint8_t>& inout) {
        int ret;
        z_stream zs;
        char buff[kSegSize];
        std::vector<uint8_t> outData;
        unsigned long prevOut = 0;

        memset(&zs, 0, sizeof(zs));
        if (inflateInit(&zs) != Z_OK) {
            return false;
        }

        zs.next_in = (Bytef *)inout.data();
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
    
    static std::vector<std::string> Split(std::string const& sentence, char delimiter) {
        std::istringstream input(sentence);
        std::vector<std::string> tokens;
        for (std::string line; std::getline(input, line, delimiter); ) {
            tokens.push_back(line);
        }
        return tokens;
    }
};
    
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

    void read(StreamReader& reader) {
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
    virtual bool readImpl(StreamReader& reader) = 0;

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
    bool readImpl(StreamReader&) override { return true; }
    uint8_t id() const override { return TAG_End; }

    static EndTag const* instance() {
        static EndTag s;
        return &s;
    }
};

namespace _internal_ {
    
template< typename T, uint8_t ID>
class ScalarTag : public Tag {
public:
    bool readImpl(StreamReader& r) override {
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

class ByteTag : public _internal_::ScalarTag<uint8_t, Tag::TAG_Byte> {};
class ShortTag : public _internal_::ScalarTag<int16_t, Tag::TAG_Short> {};
class IntTag : public _internal_::ScalarTag<int32_t, Tag::TAG_Int> {};
class LongTag : public _internal_::ScalarTag<int64_t, Tag::TAG_Long> {};


class FloatTag : public Tag {
public:
    bool readImpl(StreamReader& r) override {
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
    bool readImpl(StreamReader& r) override {
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

namespace _internal_ {
    
template<typename T, uint8_t ID>
class VectorTag : public Tag {
public:
    bool readImpl(StreamReader& r) override {
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
    
class ByteArrayTag : public _internal_::VectorTag<uint8_t, Tag::TAG_Byte_Array> {};
class IntArrayTag : public _internal_::VectorTag<int32_t, Tag::TAG_Int_Array> {};
class LongArrayTag : public _internal_::VectorTag<int64_t, Tag::TAG_Long_Array> {};


class StringTag : public Tag {
public:
    bool readImpl(StreamReader& r) override {
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
    bool readImpl(StreamReader& r) override {
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
    bool readImpl(StreamReader& r) override {
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


class Block {
public:
    explicit Block(std::string const& name, std::map<std::string, std::string> const& properties)
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
        if (offsetX < 0 || 16 <= offsetX || offsetY < 0 || 16 <= offsetY || offsetZ < 0 || 16 <= offsetZ) {
            return nullptr;
        }
        size_t const numBits = fBlockStates.size() * 64;
        if (numBits % 4096 != 0) {
            return nullptr;
        }
        size_t const bitsPerIndex = numBits / 4096;
        int64_t const mask = std::numeric_limits<uint64_t>::max() >> (64 - bitsPerIndex);
        
        size_t const index = (size_t)offsetY * 16 * 16 + (size_t)offsetZ * 16 + (size_t)offsetX;
        size_t const bitIndex = index * bitsPerIndex;
        size_t const uint64Index = bitIndex / 64;

        if (fBlockStates.size() <= uint64Index) {
            return nullptr;
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
            return nullptr;
        }
        
        return fPalette[paletteIndex];
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
        
        auto blockStatesTag = section->query("BlockStates")->asLongArray();
        if (!blockStatesTag) {
            return nullptr;
        }
        
        return std::shared_ptr<ChunkSection>(new ChunkSection((int)yTag->fValue,
                                                              palette,
                                                              blockStatesTag->fValue));
    }
    
private:
    ChunkSection(int y, std::vector<std::shared_ptr<Block>> const& palette, std::vector<int64_t> const& blockStates)
        : fY(y)
        , fPalette(palette)
        , fBlockStates(blockStates)
    {
    }

public:
    int const fY;
    std::vector<std::shared_ptr<Block>> const fPalette;
    std::vector<int64_t> const fBlockStates;
};

    
class Chunk {
public:
    std::shared_ptr<Block> blockAt(int x, int y, int z) const {
        int const chunkX = x / 16;
        int const chunkZ = z / 16;
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return nullptr;
        }
        if (y < 0 || 256 <= y) {
            return nullptr;
        }
        int const sectionY = y / 16;
        auto sectionIt = std::find_if(fSections.begin(), fSections.end(), [sectionY](auto const& section) {
            return section->fY == sectionY;
        });
        if (sectionIt == fSections.end()) {
            return nullptr;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        int const offsetY = y - sectionY * 16;
        return (*sectionIt)->blockAt(offsetX, offsetY, offsetZ);
    }

    int minX() const { return fChunkX * 16; }
    int maxX() const { return fChunkX * 16 + 15; }

    int minZ() const { return fChunkZ * 16; }
    int maxZ() const { return fChunkZ * 16 + 15; }

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
        , fSections(sections)
    {
    }

public:
    int const fChunkX;
    int const fChunkZ;
    std::vector<std::shared_ptr<ChunkSection>> fSections;
};


class ChunkDataSource {
public:
    ChunkDataSource(int chunkX, int chunkZ, uint32_t timestamp, long offset, long length)
        : fChunkX(chunkX)
        , fChunkZ(chunkZ)
        , fTimestamp(timestamp)
        , fOffset(offset)
        , fLength(length)
    {
    }
    
    bool load(StreamReader& reader, std::function<void(Chunk const& chunk)> callback) const {
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


class Region {
public:
    Region(Region const&) = delete;
    Region& operator = (Region const&) = delete;

    using LoadChunkDataCallback = std::function<bool(ChunkDataSource data, StreamReader& stream)>;
    
    bool loadChunkDataSources(LoadChunkDataCallback callback) {
        auto fs = std::make_shared<FileStream>(fFilePath);
        StreamReader sr(fs);
        for (int z = 0; z < 32; z++) {
            for (int x = 0; x < 32; x++) {
                if (!loadChunkDataSourceImpl(x, z, sr, callback)) {
                    return false;
                }
            }
        }

        return true;
    }
    
    bool loadChunkDataSource(int regionX, int regionZ, LoadChunkDataCallback callback) {
        if (regionX < 0 || 32 <= regionX || regionZ < 0 || 32 <= regionZ) {
            return false;
        }
        auto fs = std::make_shared<FileStream>(fFilePath);
        StreamReader sr(fs);
        return loadChunkDataSourceImpl(regionX, regionZ, sr, callback);
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
        
        std::vector<std::string> tokens = String::Split(basename, '.');
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
    
private:
    Region(std::string const& filePath, int x, int z)
        : fX(x)
        , fZ(z)
        , fFilePath(filePath)
    {
    }

    bool loadChunkDataSourceImpl(int regionX, int regionZ, StreamReader& sr, LoadChunkDataCallback callback) {
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
        ChunkDataSource data(chunkX, chunkZ, timestamp, sectorOffset * kSectorSize, chunkSize);
        if (!callback(data, sr)) {
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
        int const blocksPerRegion = 16 * 32;
        int const minRegionX = minX / blocksPerRegion;
        int const maxRegionX = (maxX + maxX % blocksPerRegion) / blocksPerRegion;
        int const minRegionZ = minZ / blocksPerRegion;
        int const maxRegionZ = (maxZ + maxZ % blocksPerRegion) / blocksPerRegion;
        for (int regionZ = minRegionZ; regionZ <= maxRegionZ; regionZ++) {
            for (int regionX = minRegionX; regionX <= maxRegionX; regionX++) {
                auto region = this->region(regionX, regionZ);
                if (!region) {
                    continue;
                }
                return region->loadChunkDataSources([=](ChunkDataSource data, StreamReader& reader) {
                    return data.load(reader, [=](Chunk const& chunk) {
                        for (int y = 0; y < 256; y++) {
                            for (int z = std::max(minZ, chunk.minZ()); z <= std::min(maxZ, chunk.maxZ()); z++) {
                                for (int x = std::max(minX, chunk.minX()); x <= std::min(maxX, chunk.maxX()); x++) {
                                    auto block = chunk.blockAt(x, y, z);
                                    if (!callback(x, y, z, block)) {
                                        return false;
                                    }
                                }
                            }
                        }
                        return true;
                    });
                });
            }
        }
        return true;
    }

public:
    std::string const fRootDirectory;
};

} // namespace mcfile
