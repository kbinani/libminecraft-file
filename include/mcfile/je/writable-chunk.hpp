#pragma once

namespace mcfile::je {

class WritableChunk : public Chunk {
public:
    static std::shared_ptr<WritableChunk> MakeChunk(int chunkX, int chunkZ, std::shared_ptr<nbt::CompoundTag> const &root) {
        auto chunk = Chunk::MakeChunk(chunkX, chunkZ, root);
        if (!chunk) {
            return nullptr;
        }
        auto ret = std::shared_ptr<WritableChunk>(new WritableChunk(*chunk, root));
        return ret;
    }

    static std::shared_ptr<WritableChunk> MakeEmpty(int chunkX, int chunkY, int chunkZ, int dataVersion = Chunk::kDataVersion) {
        using namespace std;
        using namespace mcfile::nbt;

        auto empty = make_shared<mcfile::je::Chunk>(chunkX, chunkY, chunkZ, dataVersion);
        auto root = make_shared<CompoundTag>();
        auto ret = shared_ptr<WritableChunk>(new WritableChunk(*empty, root));
        return ret;
    }

    std::shared_ptr<nbt::CompoundTag> toCompoundTag() const {
        if (fDataVersion <= 2840) {
            return toCompoundTag112();
        } else {
            return toCompoundTag118();
        }
    }

    bool write(stream::OutputStream &s) const {
        auto compound = toCompoundTag();
        if (!compound) {
            return false;
        }
        return nbt::CompoundTag::WriteCompressed(*compound, s, Endian::Big);
    }

    std::shared_ptr<nbt::CompoundTag> toEntitiesCompoundTag() const {
        using namespace mcfile::nbt;
        auto c = std::make_shared<CompoundTag>();
        auto entities = std::make_shared<ListTag>(Tag::Type::Compound);
        for (auto const &it : fEntities) {
            entities->push_back(it);
        }
        c->set("Entities", entities);
        return c;
    }

    bool writeEntities(stream::OutputStream &s) const {
        using namespace mcfile::nbt;
        auto c = toEntitiesCompoundTag();
        if (!c) {
            return false;
        }
        return CompoundTag::WriteCompressed(*c, s, Endian::Big);
    }

    void dataVersion(int version) {
        fDataVersion = version;
    }

private:
    WritableChunk(Chunk &s, std::shared_ptr<nbt::CompoundTag> const &root)
        : Chunk(s)
        , fRoot(root) {
    }

    std::shared_ptr<nbt::CompoundTag> toCompoundTag118() const {
        using namespace std;
        using namespace mcfile::nbt;

        static unordered_set<string> const sExclude = {
            "DataVersion",
            "Status",
            "xPos",
            "yPos",
            "zPos",
            "LastUpdate",
            "block_entities",
            "structures",
            "sections",
            "Heightmaps",
        };
        auto level = make_shared<CompoundTag>();
        if (fRoot) {
            for (auto it : *fRoot) {
                if (sExclude.find(it.first) != sExclude.end()) {
                    continue;
                }
                level->set(it.first, it.second->clone());
            }
        }

        level->set("DataVersion", make_shared<IntTag>(fDataVersion));
        level->set("Status", make_shared<StringTag>(fStatus));
        level->set("xPos", make_shared<IntTag>(fChunkX));
        level->set("zPos", make_shared<IntTag>(fChunkZ));
        level->set("yPos", make_shared<IntTag>(fChunkY));
        level->set("LastUpdate", make_shared<LongTag>(fLastUpdate));

        auto blockEntities = make_shared<ListTag>(Tag::Type::Compound);
        for (auto const &it : fTileEntities) {
            blockEntities->push_back(it.second->clone());
        }
        level->set("block_entities", blockEntities);

        if (fStructures) {
            level->set("structures", fStructures->clone());
        }

        if (!fLiquidTicks.empty()) {
            auto lt = make_shared<ListTag>(Tag::Type::Compound);
            for (TickingBlock const &tb : fLiquidTicks) {
                lt->push_back(tb.toCompoundTag());
            }
            level->set("fluid_ticks", lt);
        }

        if (!fTileTicks.empty()) {
            auto tt = make_shared<ListTag>(Tag::Type::Compound);
            for (TickingBlock const &tb : fTileTicks) {
                tt->push_back(tb.toCompoundTag());
            }
            level->set("block_ticks", tt);
        }

        vector<shared_ptr<ChunkSection>> sections;
        for (auto const &section : fSections) {
            if (section) {
                sections.push_back(section);
            }
        }
        if (!sections.empty()) {
            sort(sections.begin(), sections.end(), [](auto const &a, auto const &b) {
                return a->rawY() < b->rawY();
            });
            auto sectionsList = make_shared<ListTag>(Tag::Type::Compound);
            for (auto const &section : sections) {
                auto s = section->toCompoundTag();
                if (!s) {
                    return nullptr;
                }
                sectionsList->push_back(s);
            }
            level->set("sections", sectionsList);
        }

        if (auto heightMaps = packHeightMap(); heightMaps) {
            level->set("Heightmaps", heightMaps);
        }

        return level;
    }

    std::shared_ptr<nbt::CompoundTag> toCompoundTag112() const {
        using namespace std;
        using namespace mcfile::nbt;
        auto root = make_shared<CompoundTag>();
        root->set("DataVersion", make_shared<IntTag>(fDataVersion));

        auto level = make_shared<CompoundTag>();

        level->set("xPos", make_shared<IntTag>(fChunkX));
        level->set("zPos", make_shared<IntTag>(fChunkZ));

        vector<shared_ptr<ChunkSection>> sections;
        for (auto const &section : fSections) {
            if (section) {
                sections.push_back(section);
            }
        }
        if (!sections.empty()) {
            sort(sections.begin(), sections.end(), [](auto const &a, auto const &b) {
                return a->rawY() < b->rawY();
            });
            auto sectionsList = make_shared<ListTag>(Tag::Type::Compound);
            for (auto const &section : sections) {
                auto s = section->toCompoundTag();
                if (!s) {
                    return nullptr;
                }
                sectionsList->push_back(s);
            }
            level->set("Sections", sectionsList);
        }

        if (fDataVersion >= 2203) { // 19w36a
            if (fLegacyBiomes.size() == 1024 || fLegacyBiomes.size() == 1536) {
                vector<int32_t> biomes;
                biomes.reserve(fLegacyBiomes.size());
                for (uint16_t b : fLegacyBiomes) {
                    biomes.push_back((uint8_t)(0xff & b));
                }
                level->set("Biomes", make_shared<IntArrayTag>(biomes));
            }
        } else {
            if (fLegacyBiomes.size() == 256) {
                vector<uint8_t> biomes;
                biomes.reserve(256);
                for (uint16_t b : fLegacyBiomes) {
                    biomes.push_back((uint8_t)(0xff & b));
                }
                level->set("Biomes", make_shared<ByteArrayTag>(biomes));
            }
        }

        auto entities = make_shared<ListTag>(Tag::Type::Compound);
        for (auto const &entity : fEntities) {
            entities->push_back(entity->clone());
        }
        level->set("Entities", entities);

        auto tileEntities = make_shared<ListTag>(Tag::Type::Compound);
        for (auto const &it : fTileEntities) {
            tileEntities->push_back(it.second->clone());
        }
        level->set("TileEntities", tileEntities);

        if (fStructures) {
            level->set("Structures", fStructures->clone());
        }

        level->set("Status", make_shared<StringTag>(fStatus));

        static set<string> const whitelist = {
            "DataVersion",
            "xPos",
            "zPos",
            "Sections",
            "Biomes",
            "Entities",
            "TileEntities",
            "Structures",
            "Status",
        };
        CompoundTag const *existingLevel = fRoot->query("/Level")->asCompound();
        if (existingLevel) {
            for (auto it : *existingLevel) {
                if (whitelist.find(it.first) != whitelist.end()) {
                    continue;
                }
                level->set(it.first, it.second->clone());
            }
        }

        root->set("Level", level);
        return root;
    }

    std::shared_ptr<nbt::CompoundTag> packHeightMap() const {
        using namespace std;
        uint16_t motionBlocking[256];
        uint16_t motionBlockingNoLeaves[256];
        uint16_t oceanFloor[256];
        uint16_t worldSurface[256];
        fill_n(motionBlocking, 256, 0xffff);
        fill_n(motionBlockingNoLeaves, 256, 0xffff);
        fill_n(oceanFloor, 256, 0xffff);
        fill_n(worldSurface, 256, 0xffff);

        int x0 = minBlockX();
        int y0 = minBlockY();
        int z0 = minBlockZ();

        int i = 0;
        for (int z = 0; z < 16; z++) {
            for (int x = 0; x < 16; x++) {
                int i = z * 16 + x;
                int count = 0;
                for (int y = maxBlockX(); y >= y0 && count < 4; y--) {
                    auto block = blockAt(x + x0, y + y0, z + z0);
                    if (!block) {
                        continue;
                    }
                    if (motionBlocking[i] == 0xffff && Heightmap::IsMotionBlocking(block->fId)) {
                        motionBlocking[i] = y - y0;
                        count++;
                    }
                    if (motionBlockingNoLeaves[i] == 0xffff && Heightmap::IsMotionBlockingNoLeaves(block->fId)) {
                        motionBlockingNoLeaves[i] = y - y0;
                        count++;
                    }
                    if (oceanFloor[i] == 0xffff && Heightmap::IsOceanFloor(block->fId)) {
                        oceanFloor[i] = y - y0;
                        count++;
                    }
                    if (worldSurface[i] == 0xffff && Heightmap::IsWorldSurface(block->fId)) {
                        worldSurface[i] = y - y0;
                        count++;
                    }
                }
                if (motionBlocking[i] == 0xffff) {
                    motionBlocking[i] = 0;
                }
                if (motionBlockingNoLeaves[i] == 0xffff) {
                    motionBlockingNoLeaves[i] = 0;
                }
                if (oceanFloor[i] == 0xffff) {
                    oceanFloor[i] = 0;
                }
                if (worldSurface[i] == 0xffff) {
                    worldSurface[i] = 0;
                }
            }
        }

        if (fDataVersion < 2529) {
            // 2504 20w06a
            // 2524 20w14a
            // 2526 20w16a
            // TODO:
            return nullptr;
        } else {
            // 2529 20w17a
            // 2532 20w18a
            // 2555 20w22a
            auto tag = make_shared<nbt::CompoundTag>();
            HeightmapV2 map;

            for (int z = 0; z < 16; z++) {
                for (int x = 0; x < 16; x++) {
                    map.setUnchecked(x, z, motionBlocking[z * 16 + x]);
                }
            }
            auto motionBlockingTag = make_shared<nbt::LongArrayTag>(37);
            copy(map.fStorage.begin(), map.fStorage.end(), motionBlockingTag->fValue.begin());
            tag->set("MOTION_BLOCKING", motionBlockingTag);

            for (int z = 0; z < 16; z++) {
                for (int x = 0; x < 16; x++) {
                    map.setUnchecked(x, z, motionBlockingNoLeaves[z * 16 + x]);
                }
            }
            auto motionBlockingNoLeavesTag = make_shared<nbt::LongArrayTag>(37);
            copy(map.fStorage.begin(), map.fStorage.end(), motionBlockingNoLeavesTag->fValue.begin());
            tag->set("MOTION_BLOCKING_NO_LEAVES", motionBlockingNoLeavesTag);

            for (int z = 0; z < 16; z++) {
                for (int x = 0; x < 16; x++) {
                    map.setUnchecked(x, z, oceanFloor[z * 16 + x]);
                }
            }
            auto oceanFloorTag = make_shared<nbt::LongArrayTag>(37);
            copy(map.fStorage.begin(), map.fStorage.end(), oceanFloorTag->fValue.begin());
            tag->set("OCEAN_FLOOR", oceanFloorTag);

            for (int z = 0; z < 16; z++) {
                for (int x = 0; x < 16; x++) {
                    map.setUnchecked(x, z, worldSurface[z * 16 + x]);
                }
            }
            auto worldSurfaceTag = make_shared<nbt::LongArrayTag>(37);
            copy(map.fStorage.begin(), map.fStorage.end(), worldSurfaceTag->fValue.begin());
            tag->set("WORLD_SURFACE", worldSurfaceTag);

            return tag;
        }
    }

public:
    std::shared_ptr<mcfile::nbt::CompoundTag> fRoot;
};

} // namespace mcfile::je
