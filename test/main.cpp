#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "minecraft-file.hpp"

#include "BlockData.hpp"
#include "Point3D.hpp"

using namespace mcfile;
namespace fs = std::filesystem;

TEST_CASE("1.13.2") {
    auto const file = fs::path(__FILE__);
    auto const dir = file.parent_path();
    std::map<Point3D, BlockData> expected;
    BlockData::ReadAll((dir / "data" / "1.13.2" / "mitomitoyapparikawaiiyo" / "region" / "r.0.0-c.0.0.csv").string(), expected);

    je::World world(dir / "data" / "1.13.2" / "mitomitoyapparikawaiiyo");

    SUBCASE("Chunk::{blockAt,blockLightAt,skyLightAt}") {
        auto region = world.region(0, 0);
        CHECK(region != nullptr);
        auto const &chunk = region->chunkAt(0, 0);
        CHECK(chunk != nullptr);
        CHECK(chunk->minBlockX() == 0);
        CHECK(chunk->maxBlockX() == 15);
        CHECK(chunk->minBlockY() == 0);
        CHECK(chunk->maxBlockY() == 255);
        CHECK(chunk->minBlockZ() == 0);
        CHECK(chunk->maxBlockZ() == 15);
        for (int y = 0; y < 256; y++) {
            for (int z = chunk->minBlockZ(); z <= chunk->maxBlockZ(); z++) {
                for (int x = chunk->minBlockX(); x <= chunk->maxBlockX(); x++) {
                    auto pos = MakePoint3D(x, y, z);
                    auto expectedIt = expected.find(pos);
                    assert(expectedIt != expected.end());
                    auto const &e = expectedIt->second;

                    auto block = chunk->blockAt(x, y, z);
                    if (block) {
                        CHECK(block->fName == e.fName);
                    } else {
                        CHECK(e.fName == "NULL");
                    }

                    auto blockLight = chunk->blockLightAt(x, y, z);
                    CHECK(blockLight == e.fBlockLight);

                    auto skyLight = chunk->skyLightAt(x, y, z);
                    CHECK(skyLight == e.fSkyLight);
                }
            }
        }
    }

    SUBCASE("Chunk::blockIdAt") {
        auto region = world.region(0, 0);
        CHECK(region != nullptr);
        auto const &chunk = region->chunkAt(0, 0);
        CHECK(chunk != nullptr);
        for (int y = 0; y < 256; y++) {
            for (int z = chunk->minBlockZ(); z <= chunk->maxBlockZ(); z++) {
                for (int x = chunk->minBlockX(); x <= chunk->maxBlockX(); x++) {
                    auto pos = MakePoint3D(x, y, z);
                    auto expectedIt = expected.find(pos);
                    assert(expectedIt != expected.end());
                    auto const &e = expectedIt->second;

                    auto blockId = chunk->blockIdAt(x, y, z);
                    if (e.fName == "NULL") {
                        CHECK(blockId == blocks::unknown);
                    } else {
                        auto ex = blocks::FromName(e.fName);
                        CHECK(blockId == ex);
                    }
                }
            }
        }
    }

    SUBCASE("World::eachBlock") {
        auto const ret = world.eachBlock(0, 0, 15, 15, [&expected](int x, int y, int z, std::shared_ptr<je::Block const> const &block) {
            auto pos = MakePoint3D(x, y, z);
            auto expectedIt = expected.find(pos);
            assert(expectedIt != expected.end());
            auto const &e = expectedIt->second;
            if (block) {
                CHECK(block->fName == e.fName);
            } else {
                CHECK(e.fName == "NULL");
            }
            return true;
        });
        CHECK(ret);
    }
}

TEST_CASE("20w06a") {
    auto const file = fs::path(__FILE__);
    auto dir = file.parent_path();
    je::World world(dir / "data" / "20w06a" / "world" / "DIM-1");

    SUBCASE("biomeAt") {
        auto const &region = world.region(-1, -1);
        CHECK(region);
        auto const &chunk = region->chunkAt(-12, -5);
        CHECK(chunk);
        {
            auto const biome = chunk->biomeAt(-190, 54, -79);
            CHECK((int)biome == (int)biomes::minecraft::crimson_forest);
        }
        {
            auto const biome = chunk->biomeAt(-180, 75, -69);
            CHECK((int)biome == (int)biomes::minecraft::soul_sand_valley);
        }
    }
}

TEST_CASE("block-id") {
    for (mcfile::blocks::BlockId id = 1; id < blocks::minecraft::minecraft_max_block_id; id++) {
        auto name = blocks::Name(id);
        CHECK(!name.empty());
        auto reverse = blocks::FromName(name);
        CHECK(reverse == id);
        if (reverse != id) {
            std::cout << name << std::endl;
        }
    }
}

namespace {
std::shared_ptr<mcfile::nbt::CompoundTag> ReadCompoundFromFile(fs::path p) {
    auto s = std::make_shared<mcfile::stream::FileInputStream>(p);
    mcfile::stream::InputStreamReader reader(s, {.fLittleEndian = false});
    auto tag = std::make_shared<mcfile::nbt::CompoundTag>();
    CHECK(tag->read(reader));
    return tag;
}

std::shared_ptr<mcfile::nbt::CompoundTag> ReadCompoundFromCompressedFile(fs::path p) {
    auto size = fs::file_size(p);
    std::vector<uint8_t> buffer(size);
    FILE *fp = mcfile::File::Open(p, mcfile::File::Mode::Read);
    fread(buffer.data(), size, 1, fp);
    mcfile::Compression::decompress(buffer);
    auto s = std::make_shared<mcfile::stream::ByteStream>(buffer);
    mcfile::stream::InputStreamReader reader(s, {.fLittleEndian = false});
    auto tag = std::make_shared<mcfile::nbt::CompoundTag>();
    CHECK(tag->read(reader));
    return tag;
}

void CheckTag(mcfile::nbt::Tag const *va, mcfile::nbt::Tag const *vb, std::unordered_set<std::string> const &ignored) {
    using namespace mcfile::nbt;
    CHECK(va->type() == vb->type());
    switch (va->type()) {
    case Tag::Type::Byte:
        CHECK(va->asByte()->fValue == vb->asByte()->fValue);
        break;
    case Tag::Type::Double:
        CHECK(va->asDouble()->fValue == vb->asDouble()->fValue);
        break;
    case Tag::Type::Float:
        CHECK(va->asFloat()->fValue == vb->asFloat()->fValue);
        break;
    case Tag::Type::Int:
        CHECK(va->asInt()->fValue == vb->asInt()->fValue);
        break;
    case Tag::Type::Long:
        CHECK(va->asLong()->fValue == vb->asLong()->fValue);
        break;
    case Tag::Type::Short:
        CHECK(va->asShort()->fValue == vb->asShort()->fValue);
        break;
    case Tag::Type::String:
        CHECK(va->asString()->fValue == vb->asString()->fValue);
        break;
    case Tag::Type::ByteArray: {
        auto const &la = va->asByteArray()->value();
        auto const &lb = vb->asByteArray()->value();
        CHECK(la.size() == lb.size());
        for (int i = 0; i < la.size(); i++) {
            CHECK(la[i] == lb[i]);
        }
        break;
    }
    case Tag::Type::IntArray: {
        auto const &la = va->asIntArray()->value();
        auto const &lb = vb->asIntArray()->value();
        CHECK(la.size() == lb.size());
        for (int i = 0; i < la.size(); i++) {
            CHECK(la[i] == lb[i]);
        }
        break;
    }
    case Tag::Type::LongArray: {
        auto const &la = va->asLongArray()->value();
        auto const &lb = vb->asLongArray()->value();
        CHECK(la.size() == lb.size());
        for (int i = 0; i < la.size(); i++) {
            CHECK(la[i] == lb[i]);
        }
        break;
    }
    case Tag::Type::List: {
        auto la = va->asList();
        auto lb = vb->asList();
        CHECK(la->size() == lb->size());
        for (int i = 0; i < la->size(); i++) {
            CheckTag(la->at(i).get(), lb->at(i).get(), ignored);
        }
        break;
    }
    case Tag::Type::Compound: {
        auto ca = va->asCompound();
        auto cb = vb->asCompound();
        CHECK(ca->size() == cb->size());
        for (auto it : *ca) {
            if (ignored.find(it.first) != ignored.end()) {
                continue;
            }
            auto a = it.second;
            auto found = cb->find(it.first);
            CHECK(found != cb->end());
            auto b = found->second;
            CheckTag(a.get(), b.get(), ignored);
        }
        break;
    }
    default:
        CHECK(false);
        break;
    }
}

} // namespace

TEST_CASE("1.18") {
    auto const file = fs::path(__FILE__);
    auto dir = file.parent_path();
    je::World world(dir / "data" / "1.18rc3" / "12345678");
    SUBCASE("blockAt") {
        auto const &chunk = world.chunkAt(0, 0);
        CHECK(chunk);
        auto bricks = chunk->blockAt(14, -52, 9);
        CHECK(bricks);
        CHECK(bricks->fName == "minecraft:bricks");
        auto stoneBricks = chunk->blockAt(11, 316, 7);
        CHECK(stoneBricks);
        CHECK(stoneBricks->fName == "minecraft:stone_bricks");
    }
    SUBCASE("biomeAt") {
        auto const &chunk = world.chunkAt(10, 17);
        CHECK(chunk->biomeAt(161, 2, 278) == biomes::minecraft::lush_caves);
        CHECK(chunk->biomeAt(161, 9, 278) == biomes::minecraft::snowy_beach);
    }
    SUBCASE("toCompoundTag") {
        auto tempDir = File::CreateTempDir(fs::temp_directory_path());
        auto region = world.region(0, 0);
        auto original = *tempDir / "original_c.0.0.nbt";
        CHECK(region->exportToNbt(0, 0, original));
        auto const &writableChunk = world.writableChunkAt(0, 0);
        auto written = *tempDir / "written_c.0.0.nbt";
        auto stream = std::make_shared<mcfile::stream::FileOutputStream>(written);
        CHECK(writableChunk->write(*stream));
        stream.reset();

        auto expected = ReadCompoundFromFile(original);
        auto actual = ReadCompoundFromCompressedFile(written);
        CHECK(actual);
        CheckTag(expected.get(), actual.get(), {"palette", "data"});

        std::error_code ec;
        fs::remove_all(*tempDir, ec);
    }
}
