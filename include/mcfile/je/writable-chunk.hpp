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
        c->set(u8"Entities", entities);
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

        static unordered_set<u8string> const sExclude = {
            u8"DataVersion",
            u8"Status",
            u8"xPos",
            u8"yPos",
            u8"zPos",
            u8"LastUpdate",
            u8"block_entities",
            u8"structures",
            u8"sections",
            u8"Heightmaps",
            u8"isLightOn",
            u8"block_ticks",
            u8"fluid_ticks",
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

        level->set(u8"DataVersion", make_shared<IntTag>(fDataVersion));
        level->set(u8"Status", make_shared<StringTag>(fStatus));
        level->set(u8"xPos", make_shared<IntTag>(fChunkX));
        level->set(u8"zPos", make_shared<IntTag>(fChunkZ));
        level->set(u8"yPos", make_shared<IntTag>(fChunkY));
        level->set(u8"LastUpdate", make_shared<LongTag>(fLastUpdate));

        auto blockEntities = make_shared<ListTag>(Tag::Type::Compound);
        for (auto const &it : fTileEntities) {
            blockEntities->push_back(it.second->clone());
        }
        level->set(u8"block_entities", blockEntities);

        if (fStructures) {
            level->set(u8"structures", fStructures->clone());
        }

        auto lt = make_shared<ListTag>(Tag::Type::Compound);
        for (TickingBlock const &tb : fLiquidTicks) {
            lt->push_back(tb.toCompoundTag());
        }
        level->set(u8"fluid_ticks", lt);

        auto tt = make_shared<ListTag>(Tag::Type::Compound);
        for (TickingBlock const &tb : fTileTicks) {
            tt->push_back(tb.toCompoundTag());
        }
        level->set(u8"block_ticks", tt);

        vector<shared_ptr<ChunkSection>> sections;
        for (auto const &section : fSections) {
            if (section) {
                sections.push_back(section);
            }
        }
        bool isLightOn = false;
        if (!sections.empty()) {
            sort(sections.begin(), sections.end(), [](auto const &a, auto const &b) {
                return a->y() < b->y();
            });
            auto sectionsList = make_shared<ListTag>(Tag::Type::Compound);
            if (sections[0]->fSkyLight.size() == 2048 && fBottomSection && fBottomSection->fSkyLight.size() == 2048 && fDataVersion >= 1901) {
                auto s = fBottomSection->toCompoundTag();
                if (!s) {
                    return nullptr;
                }
                sectionsList->push_back(s);
                isLightOn = true;
            }
            for (auto const &section : sections) {
                auto s = section->toCompoundTag();
                if (!s) {
                    return nullptr;
                }
                sectionsList->push_back(s);
                if (!section->fSkyLight.empty() || !section->fBlockLight.empty()) {
                    isLightOn = true;
                }
            }
            level->set(u8"sections", sectionsList);
        }
        level->set(u8"isLightOn", make_shared<ByteTag>(isLightOn ? 1 : 0));

        if (auto heightMaps = packHeightMap(); heightMaps) {
            level->set(u8"Heightmaps", heightMaps);
        }

        return level;
    }

    std::shared_ptr<nbt::CompoundTag> toCompoundTag112() const {
        using namespace std;
        using namespace mcfile::nbt;
        auto root = make_shared<CompoundTag>();
        root->set(u8"DataVersion", make_shared<IntTag>(fDataVersion));

        auto level = make_shared<CompoundTag>();

        level->set(u8"xPos", make_shared<IntTag>(fChunkX));
        level->set(u8"zPos", make_shared<IntTag>(fChunkZ));

        vector<shared_ptr<ChunkSection>> sections;
        for (auto const &section : fSections) {
            if (section) {
                sections.push_back(section);
            }
        }
        if (!sections.empty()) {
            sort(sections.begin(), sections.end(), [](auto const &a, auto const &b) {
                return a->y() < b->y();
            });
            auto sectionsList = make_shared<ListTag>(Tag::Type::Compound);
            for (auto const &section : sections) {
                auto s = section->toCompoundTag();
                if (!s) {
                    return nullptr;
                }
                sectionsList->push_back(s);
            }
            level->set(u8"Sections", sectionsList);
        }

        if (fDataVersion >= 2203) { // 19w36a
            if (fLegacyBiomes.size() == 1024 || fLegacyBiomes.size() == 1536) {
                vector<int32_t> arr;
                arr.reserve(fLegacyBiomes.size());
                for (biomes::BiomeId b : fLegacyBiomes) {
                    if (auto id = biomes::Biome::ToInt(b); id) {
                        arr.push_back(*id);
                    } else {
                        arr.push_back(0);
                    }
                }
                level->set(u8"Biomes", make_shared<IntArrayTag>(arr));
            }
        } else {
            if (fLegacyBiomes.size() == 256) {
                vector<uint8_t> arr;
                arr.reserve(256);
                for (biomes::BiomeId b : fLegacyBiomes) {
                    if (auto id = biomes::Biome::ToInt(b); id) {
                        arr.push_back(*id);
                    } else {
                        arr.push_back(0);
                    }
                }
                level->set(u8"Biomes", make_shared<ByteArrayTag>(arr));
            }
        }

        auto entities = make_shared<ListTag>(Tag::Type::Compound);
        for (auto const &entity : fEntities) {
            entities->push_back(entity->clone());
        }
        level->set(u8"Entities", entities);

        auto tileEntities = make_shared<ListTag>(Tag::Type::Compound);
        for (auto const &it : fTileEntities) {
            if (it.second) {
                tileEntities->push_back(it.second->clone());
            }
        }
        level->set(u8"TileEntities", tileEntities);

        if (fStructures) {
            level->set(u8"Structures", fStructures->clone());
        }

        level->set(u8"Status", make_shared<StringTag>(fStatus));

        static set<u8string> const whitelist = {
            u8"DataVersion",
            u8"xPos",
            u8"zPos",
            u8"Sections",
            u8"Biomes",
            u8"Entities",
            u8"TileEntities",
            u8"Structures",
            u8"Status",
        };
        CompoundTag const *existingLevel = fRoot->query(u8"/Level")->asCompound();
        if (existingLevel) {
            for (auto it : *existingLevel) {
                if (whitelist.find(it.first) != whitelist.end()) {
                    continue;
                }
                level->set(it.first, it.second->clone());
            }
        }

        root->set(u8"Level", level);
        return root;
    }

    std::shared_ptr<nbt::CompoundTag> packHeightMap() const {
        using namespace std;

        uint16_t const undefined = 0xffff;

        uint16_t motionBlocking[256];
        uint16_t motionBlockingNoLeaves[256];
        uint16_t oceanFloor[256];
        uint16_t worldSurface[256];
        fill_n(motionBlocking, 256, undefined);
        fill_n(motionBlockingNoLeaves, 256, undefined);
        fill_n(oceanFloor, 256, undefined);
        fill_n(worldSurface, 256, undefined);

        int x0 = minBlockX();
        int y0 = minBlockY();
        int z0 = minBlockZ();

        for (int z = 0; z < 16; z++) {
            for (int x = 0; x < 16; x++) {
                int i = z * 16 + x;
                int count = 0;
                for (int y = maxBlockY(); y >= y0; y--) {
                    auto block = blockAt(x + x0, y, z + z0);
                    if (!block) {
                        continue;
                    }
                    if (motionBlocking[i] == undefined) {
                        if (Heightmap::IsMotionBlocking(*block)) {
                            motionBlocking[i] = y - y0 + 1;
                            count++;
                        }
                    }
                    if (motionBlockingNoLeaves[i] == undefined) {
                        if (Heightmap::IsMotionBlockingNoLeaves(*block)) {
                            motionBlockingNoLeaves[i] = y - y0 + 1;
                            count++;
                        }
                    }
                    if (oceanFloor[i] == undefined) {
                        if (Heightmap::IsOceanFloor(*block)) {
                            oceanFloor[i] = y - y0 + 1;
                            count++;
                        }
                    }
                    if (worldSurface[i] == undefined) {
                        if (Heightmap::IsWorldSurface(*block)) {
                            worldSurface[i] = y - y0 + 1;
                            count++;
                        }
                    }
                    if (count >= 4) {
                        break;
                    }
                }
                if (motionBlocking[i] == undefined) {
                    motionBlocking[i] = 0;
                }
                if (motionBlockingNoLeaves[i] == undefined) {
                    motionBlockingNoLeaves[i] = 0;
                }
                if (oceanFloor[i] == undefined) {
                    oceanFloor[i] = 0;
                }
                if (worldSurface[i] == undefined) {
                    worldSurface[i] = 0;
                }
            }
        }

        shared_ptr<Heightmap> map;
        if (fDataVersion < 2529) {
            // 2504 20w06a
            // 2524 20w14a
            // 2526 20w16a
            map = make_shared<HeightmapV1>();
        } else {
            // 2529 20w17a
            // 2532 20w18a
            // 2555 20w22a
            map = make_shared<HeightmapV2>();
        }

        auto tag = make_shared<nbt::CompoundTag>();

        map->copyFrom(motionBlocking);
        auto motionBlockingTag = make_shared<nbt::LongArrayTag>();
        map->copyTo(motionBlockingTag->fValue);
        tag->set(u8"MOTION_BLOCKING", motionBlockingTag);

        map->copyFrom(motionBlockingNoLeaves);
        auto motionBlockingNoLeavesTag = make_shared<nbt::LongArrayTag>();
        map->copyTo(motionBlockingNoLeavesTag->fValue);
        tag->set(u8"MOTION_BLOCKING_NO_LEAVES", motionBlockingNoLeavesTag);

        map->copyFrom(oceanFloor);
        auto oceanFloorTag = make_shared<nbt::LongArrayTag>();
        map->copyTo(oceanFloorTag->fValue);
        tag->set(u8"OCEAN_FLOOR", oceanFloorTag);

        map->copyFrom(worldSurface);
        auto worldSurfaceTag = make_shared<nbt::LongArrayTag>(37);
        map->copyTo(worldSurfaceTag->fValue);
        tag->set(u8"WORLD_SURFACE", worldSurfaceTag);

        return tag;
    }

public:
    std::shared_ptr<mcfile::nbt::CompoundTag> fRoot;
};

} // namespace mcfile::je
