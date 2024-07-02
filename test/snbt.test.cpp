#include <doctest/doctest.h>

#include "minecraft-file.hpp"

using namespace mcfile;
using namespace mcfile::nbt;
using namespace std;

namespace fs = std::filesystem;

TEST_CASE("snbt") {
    SUBCASE("basic") {
        ostringstream out;
        auto nbt = make_shared<CompoundTag>();
        nbt->set(u8"a", make_shared<ByteTag>(249));
        nbt->set(u8"b", make_shared<ShortTag>(2));
        nbt->set(u8"c", make_shared<IntTag>(3));
        nbt->set(u8"d", make_shared<LongTag>(4));
        nbt->set(u8"e", make_shared<FloatTag>(5));
        nbt->set(u8"f", make_shared<DoubleTag>(6));
        auto byteArray = make_shared<ByteArrayTag>();
        byteArray->fValue.push_back(1);
        byteArray->fValue.push_back(0);
        byteArray->fValue.push_back(249);
        nbt->set(u8"g", byteArray);
        auto intArray = make_shared<IntArrayTag>();
        intArray->fValue.push_back(8);
        intArray->fValue.push_back(-8);
        nbt->set(u8"h", intArray);
        auto longArray = make_shared<LongArrayTag>();
        longArray->fValue.push_back(9);
        longArray->fValue.push_back(-9);
        nbt->set(u8"i", longArray);
        nbt->set(u8"j", make_shared<StringTag>(u8"foo"));
        auto list = make_shared<ListTag>(Tag::Type::Int);
        list->push_back(make_shared<IntTag>(10));
        list->push_back(make_shared<IntTag>(-10));
        nbt->set(u8"k", list);
        auto compound = make_shared<CompoundTag>();
        compound->set(u8"[wa]", make_shared<FloatTag>(3.14));
        nbt->set(u8"l", compound);

        nbt->toSnbt(out, {});

        auto s = out.str();
        CHECK(s == R"({a:-7b,b:2s,c:3,d:4l,e:5f,f:6d,g:[B;1b,0b,-7b],h:[I;8,-8],i:[L;9l,-9l],j:"foo",k:[10,-10],l:{"[wa]":3.14f}})");

        istringstream in(R"({a:-7b,b:2s,c:3,d:4l,e:5f,f:6d,g:[B;true,false,-7b],h:[I;8,-8],i:[L;9l,-9l],j:"foo",k:[10,-10],l:{"[wa]":3.14f}})");
        auto r = CompoundTag::FromSnbt(in);
        REQUIRE(r);
        CHECK(r->equals(*nbt));
    }
    SUBCASE("empty compound tag") {
        auto s = CompoundTag::FromSnbt(R"({})");
        CHECK(s);
    }
    SUBCASE("empty list tag") {
        auto s = CompoundTag::FromSnbt(R"({a:[]})");
        REQUIRE(s);
        auto a = s->listTag(u8"a");
        REQUIRE(a);
        CHECK(a->empty());
    }
    SUBCASE("empty byte array tag") {
        auto s = CompoundTag::FromSnbt(R"({a:[B;]})");
        REQUIRE(s);
        auto a = s->byteArrayTag(u8"a");
        REQUIRE(a);
        CHECK(a->fValue.empty());
    }
    SUBCASE("empty int array tag") {
        auto s = CompoundTag::FromSnbt(R"({a:[I;]})");
        REQUIRE(s);
        auto a = s->intArrayTag(u8"a");
        REQUIRE(a);
        CHECK(a->fValue.empty());
    }
    SUBCASE("empty long array tag") {
        auto s = CompoundTag::FromSnbt(R"({a:[L;]})");
        REQUIRE(s);
        auto a = s->longArrayTag(u8"a");
        REQUIRE(a);
        CHECK(a->fValue.empty());
    }
    SUBCASE("numeric key") {
        auto s = CompoundTag::FromSnbt(R"({0:1})");
        REQUIRE(s);
        auto c = dynamic_pointer_cast<CompoundTag>(s);
        REQUIRE(c);
        CHECK(c->int32(u8"0") == 1);
    }
}
