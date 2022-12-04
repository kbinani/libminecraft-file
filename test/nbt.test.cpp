#include <doctest/doctest.h>

#include "minecraft-file.hpp"

using namespace mcfile;
using namespace mcfile::nbt;
using namespace std;

TEST_CASE("nbt") {
    auto a = make_shared<CompoundTag>();
    a->set("byte", make_shared<ByteTag>(1));
    a->set("short", make_shared<ShortTag>(1));
    a->set("int", make_shared<IntTag>(1));
    a->set("long", make_shared<LongTag>(1));
    a->set("float", make_shared<FloatTag>(1));
    a->set("double", make_shared<DoubleTag>(1));
    auto bytearray = make_shared<ByteArrayTag>();
    bytearray->fValue.push_back(0);
    bytearray->fValue.push_back(1);
    bytearray->fValue.push_back(2);
    a->set("bytearray", bytearray);
    a->set("string", make_shared<StringTag>("foo"));
    auto list = make_shared<ListTag>(Tag::Type::String);
    list->push_back(make_shared<StringTag>("bar"));
    a->set("list", list);
    auto intarray = make_shared<IntArrayTag>();
    intarray->fValue.push_back(0);
    intarray->fValue.push_back(1);
    intarray->fValue.push_back(2);
    a->set("intarray", intarray);
    auto longarray = make_shared<LongArrayTag>();
    longarray->fValue.push_back(0);
    longarray->fValue.push_back(1);
    longarray->fValue.push_back(2);
    a->set("longarray", longarray);
    a->set("compound", a->clone());

    auto b = dynamic_pointer_cast<CompoundTag>(a->clone());
    CHECK(b->equals(*a));
    b->set("extra", make_shared<IntTag>(3));
    CHECK(!b->equals(*a));
    b->erase("extra");
    CHECK(b->equals(*a));
    b->set("int", make_shared<IntTag>(100));
    CHECK(!b->equals(*a));
}
