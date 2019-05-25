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
    explicit FileStream(std::string const& file_path)
        : fp(nullptr)
        , loc(0)
    {
        FILE *fp = fopen(file_path.c_str(), "rb");

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
        this->fp = fp;
        this->_length = length;
    }

    ~FileStream() {
        if (fp) {
            fclose(fp);
        }
    }

    FileStream(FileStream const&) = delete;
    FileStream& operator = (FileStream const&) = delete;

    bool read(void *buffer, size_t size, size_t count) override {
        if (!fp) {
            return false;
        }
        loc += size * count;
        return fread(buffer, size, count, fp) == count;
    }

    bool seek(long offset) override {
        if (!fp) {
            return false;
        }
        loc = offset;
        return fseek(fp, offset, SEEK_SET) == 0;
    }

    bool valid() const override {
        return fp != nullptr;
    }

    long length() const override { return _length; }

    long pos() const override { return loc; }

private:
    FILE *fp;
    long _length;
    long loc;
};

class ByteStream : public Stream {
public:
    explicit ByteStream(std::vector<uint8_t> const& buffer)
        : buffer(buffer)
        , loc(0)
    {
    }

    ~ByteStream() {}

    ByteStream(ByteStream const&) = delete;
    ByteStream& operator = (ByteStream const&) = delete;

    bool read(void *buf, size_t size, size_t count) override {
        if (buffer.size() <= loc + size * count) {
            return false;
        }
        std::copy(buffer.begin() + loc, buffer.begin() + loc + size * count, (uint8_t *)buf);
        loc += size * count;
        return true;
    }

    bool seek(long offset) override {
        if (offset < 0 || buffer.size() <= offset) {
            return false;
        }
        loc = offset;
        return true;
    }

    long length() const override { return buffer.size(); }

    bool valid() const override { return true; }

    long pos() const override { return loc; }

private:
    std::vector<uint8_t> buffer;
    long loc;
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
        int16_t t;
        if (!readRaw(&t)) {
            return false;
        }
        *v = Int16FromBE(t);
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
        int32_t t;
        if (!readRaw(&t)) {
            return false;
        }
        *v = Int32FromBE(t);
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
        int64_t t;
        if (!readRaw(&t)) {
            return false;
        }
        *v = Int64FromBE(t);
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

    template<typename T>
    static T SwapInt64(T v) {
        static_assert(sizeof(T) == 8, "");
        return (((T) (v) & (T) 0x00000000000000ff) << 56)
            | (((T) (v) & (T) 0x000000000000ff00) << 40)
            | (((T) (v) & (T) 0x0000000000ff0000) << 24)
            | (((T) (v) & (T) 0x00000000ff000000) <<  8)
            | (((T) (v) & (T) 0x000000ff00000000) >>  8)
            | (((T) (v) & (T) 0x0000ff0000000000) >> 24)
            | (((T) (v) & (T) 0x00ff000000000000) >> 40)
            | (((T) (v) & (T) 0xff00000000000000) >> 56);
    }

    template<typename T>
    static T SwapInt32(T v) {
        static_assert(sizeof(T) == 4, "");
        T r;
        r  = v                << 24;
        r |= (v & 0x0000FF00) <<  8;
        r |= (v & 0x00FF0000) >>  8;
        r |= v                >> 24;
        return r;
    }

    template<typename T>
    static T SwapInt16(T v) {
        static_assert(sizeof(T) == 2, "");
        T r;
        r = v << 8;
        r |= (v & 0xFF00) >> 8;
        return r;
    }

    template<typename T>
    static T Int64FromBE(T v) {
        static_assert(sizeof(T) == 8, "");
        #if __BYTE_ORDER == __ORDER_BIG_ENDIAN__
            return v;
        #else
            return SwapInt64(v);
        #endif
    }

    template<typename T>
    static T Int32FromBE(T v) {
        static_assert(sizeof(T) == 4, "");
        #if __BYTE_ORDER == __ORDER_BIG_ENDIAN__
            return v;
        #else
            return SwapInt32(v);
        #endif
    }

    template<typename T>
    static T Int16FromBE(T v) {
        static_assert(sizeof(T) == 2, "");
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
        char buff[SEG_SIZE];
        std::vector<uint8_t> out_data;

        memset(&zs, 0, sizeof(zs));
        if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK) {
            return false;
        }
        zs.next_in = (Bytef *)inout.data();
        zs.avail_in = inout.size();

        int ret;
        do {
            zs.next_out = reinterpret_cast<Bytef *>(buff);
            zs.avail_out = SEG_SIZE;

            ret = deflate(&zs, Z_FINISH);
            out_data.insert(out_data.end(), buff, buff + zs.total_out);
        } while (ret == Z_OK);

        int r = deflateEnd(&zs);
        if (ret != Z_STREAM_END) {
            return false;
        }
        if (r != Z_OK) {
            return false;
        }

        inout.swap(out_data);
        return true;
    }

    static bool decompress(std::vector<uint8_t>& inout) {
        int ret;
        z_stream zs;
        char buff[SEG_SIZE];
        std::vector<uint8_t> out_data;
        unsigned long prev_out = 0;

        memset(&zs, 0, sizeof(zs));
        if (inflateInit(&zs) != Z_OK) {
            return false;
        }

        zs.next_in = (Bytef *)inout.data();
        zs.avail_in = inout.size();

        do {
            zs.next_out = reinterpret_cast<Bytef *>(buff);
            zs.avail_out = SEG_SIZE;

            ret = inflate(&zs, 0);
            out_data.insert(out_data.end(), buff, buff + (zs.total_out - prev_out));
            prev_out = zs.total_out;
        } while (ret == Z_OK);

        inflateEnd(&zs);
        if (ret != Z_STREAM_END) {
            return false;
        }

        inout.swap(out_data);
        return true;
    }

private:
    static const unsigned int SEG_SIZE = 16384;
};

namespace nbt {

class Tag;

class TagFactory {
public:
    static inline std::shared_ptr<Tag> makeTag(int8_t id, std::string const& name);

private:
    template<typename T>
    static std::shared_ptr<T> makeNamedTag(std::string const& name) {
        auto p = std::make_shared<T>();
        p->fName = name;
        return p;
    }
};

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

    virtual int8_t id() const = 0;

    std::string name() const { return fName; }

protected:
    virtual bool readImpl(StreamReader& reader) = 0;

public:
    static int8_t const TAG_End = 0;
    static int8_t const TAG_Byte = 1;
    static int8_t const TAG_Short = 2;
    static int8_t const TAG_Int = 3;
    static int8_t const TAG_Long = 4;
    static int8_t const TAG_Float = 5;
    static int8_t const TAG_Double = 6;
    static int8_t const TAG_Byte_Array = 7;
    static int8_t const TAG_String = 8;
    static int8_t const TAG_List = 9;
    static int8_t const TAG_Compound = 10;
    static int8_t const TAG_Int_Array = 11;
    static int8_t const TAG_Long_Array = 12;

protected:
    std::string fName;

private:
    bool fValid;
};

class EndTag : public Tag {
public:
    bool readImpl(StreamReader&) override { return true; }
    int8_t id() const override { return TAG_End; }
};

template< typename T, int8_t ID>
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

    int8_t id() const override { return ID; }

public:
    T fValue;
};

class ByteTag : public ScalarTag<int8_t, Tag::TAG_Byte> {};
class ShortTag : public ScalarTag<int16_t, Tag::TAG_Short> {};
class IntTag : public ScalarTag<int32_t, Tag::TAG_Int> {};
class LongTag : public ScalarTag<int64_t, Tag::TAG_Long> {};

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

    int8_t id() const override { return Tag::TAG_Float; }

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

    int8_t id() const override { return Tag::TAG_Double; }

public:
    double fValue;
};

template<typename T, int8_t ID>
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

    int8_t id() const override { return ID; }

public:
    std::vector<T> fValue;
};

class ByteArrayTag : public VectorTag<int8_t, Tag::TAG_Byte_Array> {};
class IntArrayTag : public VectorTag<int32_t, Tag::TAG_Int_Array> {};
class LongArrayTag : public VectorTag<int64_t, Tag::TAG_Long_Array> {};

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

    int8_t id() const override { return Tag::TAG_String; }

public:
    std::string fValue;
};

class ListTag : public Tag {
public:
    bool readImpl(StreamReader& r) override {
        int8_t type;
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

    int8_t id() const override { return Tag::TAG_List; }

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
                return true;
            }
            if (type == Tag::TAG_End) {
                return true;
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

    int8_t id() const override { return Tag::TAG_Compound; }

public:
    std::map<std::string, std::shared_ptr<Tag>> fValue;
};

inline std::shared_ptr<Tag> TagFactory::makeTag(int8_t id, std::string const& name) {
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

class ChunkDataSource {
public:
    ChunkDataSource(std::string const& file_path, uint32_t timestamp, long offset, long length)
        : file_path(file_path)
        , timestamp(timestamp)
        , offset(offset)
        , length(length)
    {
    }

    bool open(std::function<void(std::vector<uint8_t> const& data)> callback) const {
        auto fs = std::make_shared<FileStream>(file_path);
        auto sr = std::make_shared<StreamReader>(fs);
        if (!sr->valid()) {
            return false;
        }
        if (!sr->seek(offset + sizeof(uint32_t))) {
            return false;
        }
        uint8_t compression_type;
        if (!sr->read(&compression_type)) {
            return false;
        }
        if (compression_type != 2) {
            return false;
        }
        std::vector<uint8_t> buffer(length - 1);
        if (!sr->read(buffer)) {
            return false;
        }
        if (!Compression::decompress(buffer)) {
            return false;
        }
        callback(buffer);
        return true;
    }

public:
    std::string const file_path;
    uint32_t const timestamp;
    long const offset;
    long const length;
};


class Region {
public:
    Region(std::string const& file_path, int x, int z)
        : x(x)
        , z(z)
        , file_path(file_path)
    {
    }

    Region(Region const&) = delete;
    Region& operator = (Region const&) = delete;

    bool loadChunkDataSource(std::function<void(ChunkDataSource const&)> callback) {
        int const x = this->x & 31;
        int const z = this->z & 31;
        int const index = x + z * 32;
        auto fs = std::make_shared<FileStream>(file_path);
        auto sr = std::make_shared<StreamReader>(fs);
        if (!sr->valid()) {
            return false;
        }
        if (!sr->seek(4 * index)) {
            return false;
        }

        uint32_t loc;
        if (!sr->read(&loc)) {
            return false;
        }

        long sectorOffset = loc >> 8;
        if (!sr->seek(kSectorSize + 4 * index)) {
            return false;
        }

        uint32_t timestamp;
        if (!sr->read(&timestamp)) {
            return false;
        }

        if (!sr->seek(sectorOffset * kSectorSize)) {
            return false;
        }
        uint32_t chunkSize;
        if (!sr->read(&chunkSize)) {
            return false;
        }

        ChunkDataSource data(file_path, timestamp, sectorOffset * kSectorSize, chunkSize);
        callback(data);

        return true;
    }

public:
    int const x;
    int const z;

private:
    std::string const file_path;

    static long const kSectorSize = 4096;
};

} // namespace mcfile
