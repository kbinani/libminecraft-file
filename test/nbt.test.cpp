#include <doctest/doctest.h>

#include "minecraft-file.hpp"

using namespace mcfile;
using namespace mcfile::nbt;
using namespace std;

namespace fs = std::filesystem;

TEST_CASE("nbt") {
    SUBCASE("basic") {
        auto a = make_shared<CompoundTag>();
        a->set(u8"byte", make_shared<ByteTag>(1));
        a->set(u8"short", make_shared<ShortTag>(1));
        a->set(u8"int", make_shared<IntTag>(1));
        a->set(u8"long", make_shared<LongTag>(1));
        a->set(u8"float", make_shared<FloatTag>(1));
        a->set(u8"double", make_shared<DoubleTag>(1));
        auto bytearray = make_shared<ByteArrayTag>();
        bytearray->fValue.push_back(0);
        bytearray->fValue.push_back(1);
        bytearray->fValue.push_back(2);
        a->set(u8"bytearray", bytearray);
        a->set(u8"string", make_shared<StringTag>(u8"foo"));
        auto list = make_shared<ListTag>(Tag::Type::String);
        list->push_back(make_shared<StringTag>(u8"bar"));
        a->set(u8"list", list);
        auto intarray = make_shared<IntArrayTag>();
        intarray->fValue.push_back(0);
        intarray->fValue.push_back(1);
        intarray->fValue.push_back(2);
        a->set(u8"intarray", intarray);
        auto longarray = make_shared<LongArrayTag>();
        longarray->fValue.push_back(0);
        longarray->fValue.push_back(1);
        longarray->fValue.push_back(2);
        a->set(u8"longarray", longarray);
        a->set(u8"compound", a->clone());

        auto b = dynamic_pointer_cast<CompoundTag>(a->clone());
        CHECK(b->equals(*a));
        b->set(u8"extra", make_shared<IntTag>(3));
        CHECK(!b->equals(*a));
        b->erase(u8"extra");
        CHECK(b->equals(*a));
        b->set(u8"int", make_shared<IntTag>(100));
        CHECK(!b->equals(*a));
    }

    SUBCASE("unicode") {
        using namespace mcfile::u8stream;
        auto const dir = fs::path(__FILE__).parent_path();
        auto s = std::make_shared<mcfile::stream::FileInputStream>(dir / "data" / "unicode.nbt");

        auto tag = CompoundTag::Read(s, mcfile::Endian::Big);
        REQUIRE(tag);

        auto actual = tag->string(u8"Text1");
        REQUIRE(actual);

        std::string expected("\x7B\x22\x74\x65\x78\x74\x22\x3A\x22\xC3\x85\x6B\xC3\xA6\x72\x65\x74\x22\x7D");
        CHECK(*actual == std::u8string((char8_t const *)expected.c_str()));
    }
}
