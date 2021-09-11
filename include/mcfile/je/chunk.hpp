#pragma once

namespace mcfile::je {

class Chunk {
public:
    std::shared_ptr<Block const> blockAt(int x, int y, int z) const {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return nullptr;
        }
        auto const &section = unsafeSectionAtBlock(y);
        if (!section) {
            return nullptr;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        int const offsetY = y - section->y() * 16;
        return section->blockAt(offsetX, offsetY, offsetZ);
    }

    std::shared_ptr<Block const> blockAt(Pos3i pos) const {
        return blockAt(pos.fX, pos.fY, pos.fZ);
    }

    blocks::BlockId blockIdAt(int x, int y, int z) const {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return blocks::unknown;
        }
        auto const &section = unsafeSectionAtBlock(y);
        if (!section) {
            return blocks::unknown;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        int const offsetY = y - section->y() * 16;
        return section->blockIdAt(offsetX, offsetY, offsetZ);
    }

    blocks::BlockId blockIdAt(Pos3i pos) const {
        return blockIdAt(pos.fX, pos.fY, pos.fZ);
    }

    bool setBlockAt(int x, int y, int z, std::shared_ptr<Block const> const &block, SetBlockOptions options = SetBlockOptions()) {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return false;
        }
        auto const &section = sectionAtBlock(y);
        if (!section) {
            return false;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        int const offsetY = y - section->y() * 16;
        if (!section->setBlockAt(offsetX, offsetY, offsetZ, block)) {
            return false;
        }
        if (options.fRemoveTileEntity) {
            return removeTileEntityAt(x, y, z);
        } else {
            return true;
        }
    }

    bool setBlockAt(Pos3i pos, std::shared_ptr<Block const> const &block, SetBlockOptions options = SetBlockOptions()) {
        return setBlockAt(pos.fX, pos.fY, pos.fZ, block, options);
    }

    bool removeTileEntityAt(int x, int y, int z) {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return false;
        }
        Pos3i pos(x, y, z);
        fTileEntities.erase(pos);
        return true;
    }

    bool removeTileEntityAt(Pos3i pos) {
        return removeTileEntityAt(pos.fX, pos.fY, pos.fZ);
    }

    int blockLightAt(int x, int y, int z) const {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return 0;
        }
        auto const &section = unsafeSectionAtBlock(y);
        if (!section) {
            return -1;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        int const offsetY = y - section->y() * 16;
        return section->blockLightAt(offsetX, offsetY, offsetZ);
    }

    int blockLightAt(Pos3i pos) const {
        return blockLightAt(pos.fX, pos.fY, pos.fZ);
    }

    int skyLightAt(int x, int y, int z) const {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return 0;
        }
        auto const &section = unsafeSectionAtBlock(y);
        if (!section) {
            return -1;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        int const offsetY = y - section->y() * 16;
        return section->skyLightAt(offsetX, offsetY, offsetZ);
    }

    int skyLightAt(Pos3i pos) const {
        return skyLightAt(pos.fX, pos.fY, pos.fZ);
    }

    biomes::BiomeId biomeAt(int x, int y, int z) const {
        if (fDataVersion >= 2203) { // 19w36a
            int const offsetX = (x - fChunkX * 16) / 4;
            int const offsetY = (y - fMinChunkSectionY * 16) / 4;
            int const offsetZ = (z - fChunkZ * 16) / 4;
            if (offsetX < 0 || 4 <= offsetX || offsetZ < 0 || 4 <= offsetZ || offsetY < 0) {
                return biomes::unknown;
            }
            int const index = offsetX + offsetZ * 4 + offsetY * 16;
            if (index < fBiomes.size()) {
                return fBiomes[index];
            } else {
                return biomes::unknown;
            }
        } else {
            int const offsetX = x - fChunkX * 16;
            int const offsetZ = z - fChunkZ * 16;

            if (offsetX < 0 || 16 <= offsetX || offsetZ < 0 || 16 <= offsetZ) {
                return biomes::unknown;
            }
            int const index = offsetZ * 16 + offsetX;
            if (index < fBiomes.size()) {
                return fBiomes[index];
            } else {
                return biomes::unknown;
            }
        }
    }

    biomes::BiomeId biomeAt(Pos3i pos) const {
        return biomeAt(pos.fX, pos.fY, pos.fZ);
    }

    biomes::BiomeId biomeAt(int x, int z) const {
        return biomeAt(x, 0, z);
    }

    int minBlockX() const { return fChunkX * 16; }
    int maxBlockX() const { return fChunkX * 16 + 15; }

    int minBlockY() const { return fMinChunkSectionY * 16; }
    int maxBlockY() const { return fMinChunkSectionY < 0 ? 319 : 255; }

    int minBlockZ() const { return fChunkZ * 16; }
    int maxBlockZ() const { return fChunkZ * 16 + 15; }

    enum class Status {
        UNKNOWN,
        FULL,
    };

    Status status() const {
        // Java Edition 1.15.2 2230 "full"
        // Java Edition 1.14.1 1957 "full"
        // Java Edition 19w02a 1921 "full"
        // Java Edition 18w47a 1912 "full"
        // Java Edition 18w46a 1910 "fullchunk"
        // Java Edition 18w45a 1908 "fullchunk"
        // Java Edition 18w44a 1907 "fullchunk"
        // Java Edition 18w43a 1901 "fullchunk"
        // Java Edition 1.13.2 1631 "postprocessed" | "fullchunk"
        // Java Edition 1.13.2-pre2 1630 "postprocessed"
        // Java Edition 1.13.2-pre1 1629 "postprocessed"
        // Java Edition 1.13.1-pre1 1626 "postprocessed"
        // ...
        // Java Edition 18w30a 1620 "postprocessed"
        // ...
        // Java Edition 1.13-pre1 1501 "postprocessed"
        // ...
        // Java Edition 18w15a 1482 "finalized"
        // Java Edition 18w14b 1481 "postprocessed"
        // Java Edition 18w14a 1479 "postprocessed"
        // Java Edition 18w11a 1478 "postprocessed"
        // Java Edition 18w08a 1470 "postprocessed"
        // ...
        // Java Edition 18w07a 1467 "postprocessed"
        // Java Edition 18w06a 1466 "postprocessed"
        // Java Edition 18w05a 1464 TerrianPopulated 1
        // ...
        // Java Edition 18w01a 1459 TerrianPopulated 1
        // ...
        // Java Edition 17w47a 1451 TerrianPopulated 1
        if (fDataVersion >= 1912) {
            if (fStatus == "full") {
                return Status::FULL;
            } else {
                return Status::UNKNOWN;
            }
        } else if (fDataVersion >= 1466) {
            if (fStatus == "postprocessed" || fStatus == "finalized" || fStatus == "fullchunk") {
                return Status::FULL;
            } else {
                return Status::UNKNOWN;
            }
        } else {
            if (fTerrianPopulated && *fTerrianPopulated) {
                return Status::FULL;
            } else {
                return Status::UNKNOWN;
            }
        }
    }

    static std::shared_ptr<Chunk> MakeChunk(int chunkX, int chunkZ, std::shared_ptr<nbt::CompoundTag> const &root) {
        using namespace std;

        auto level = root->query("/Level")->asCompound();
        if (!level) {
            return nullptr;
        }

        int dataVersion = 0;
        auto dataVersionTag = root->query("/DataVersion")->asInt();
        if (dataVersionTag) {
            // *.mca created by Minecraft 1.2.1 does not have /DataVersion tag
            dataVersion = dataVersionTag->fValue;
        }

        vector<shared_ptr<nbt::CompoundTag>> tileEntities;
        auto tileEntitiesList = level->listTag("TileEntities");
        if (tileEntitiesList && tileEntitiesList->fType == nbt::Tag::Type::Compound) {
            for (auto it : *tileEntitiesList) {
                auto comp = it->asCompound();
                if (!comp) {
                    continue;
                }
                auto copy = make_shared<nbt::CompoundTag>();
                copy->fValue = comp->fValue;
                tileEntities.push_back(copy);
            }
        }

        auto sectionsTag = level->listTag("Sections");
        if (!sectionsTag) {
            return nullptr;
        }
        vector<shared_ptr<ChunkSection>> sections;
        auto createEmptySection = chunksection::ChunkSectionGenerator::MakeChunkSections(sectionsTag, dataVersion, chunkX, chunkZ, tileEntities, sections);

        vector<biomes::BiomeId> biomes;
        auto biomesTag = level->query("Biomes");
        ParseBiomes(biomesTag, biomes);

        vector<shared_ptr<nbt::CompoundTag>> entities;
        auto entitiesTag = level->listTag("Entities");
        if (entitiesTag && entitiesTag->fType == nbt::Tag::Type::Compound) {
            auto entitiesList = entitiesTag->asList();
            for (auto it : *entitiesList) {
                auto comp = it->asCompound();
                if (!comp) {
                    continue;
                }
                auto copy = make_shared<nbt::CompoundTag>();
                copy->fValue = comp->fValue;
                entities.push_back(copy);
            }
        }

        string s;
        auto status = level->stringTag("Status");
        if (status) {
            s = status->fValue;
        }
        std::optional<bool> terrianPopulated = level->boolean("TerrianPopulated");

        auto structures = level->compoundTag("Structures");

        auto lastUpdateTag = level->int64("LastUpdate");
        int64_t lastUpdate = 0;
        if (lastUpdateTag) {
            lastUpdate = *lastUpdateTag;
        }

        auto tileTicksTag = level->listTag("TileTicks");
        std::vector<TickingBlock> tileTicks;
        if (tileTicksTag) {
            for (auto it : *tileTicksTag) {
                auto item = it->asCompound();
                if (!item) {
                    continue;
                }
                auto tb = TickingBlock::FromCompound(*item);
                if (!tb) {
                    continue;
                }
                tileTicks.push_back(*tb);
            }
        }

        auto liquidTicksTag = level->listTag("LiquidTicks");
        std::vector<TickingBlock> liquidTicks;
        if (liquidTicksTag) {
            for (auto it : *liquidTicksTag) {
                auto item = it->asCompound();
                if (!item) {
                    continue;
                }
                auto tb = TickingBlock::FromCompound(*item);
                if (!tb) {
                    continue;
                }
                liquidTicks.push_back(*tb);
            }
        }

        return std::shared_ptr<Chunk>(new Chunk(chunkX, chunkZ, sections, dataVersion,
                                                biomes, entities, tileEntities, structures, lastUpdate,
                                                tileTicks, liquidTicks, s, terrianPopulated, createEmptySection));
    }

    static std::shared_ptr<Chunk> LoadFromCompressedChunkNbtFile(std::filesystem::path const &filePath, int chunkX, int chunkZ) {
        auto stream = std::make_shared<mcfile::stream::FileInputStream>(filePath);
        if (!stream->valid()) {
            return nullptr;
        }
        std::vector<uint8_t> buffer(stream->length());
        if (!stream->read(buffer.data(), 1, buffer.size())) {
            return nullptr;
        }
        if (!Compression::decompress(buffer)) {
            return nullptr;
        }
        auto root = std::make_shared<mcfile::nbt::CompoundTag>();
        auto bs = std::make_shared<mcfile::stream::ByteStream>(buffer);
        stream::InputStreamReader reader(bs);
        root->read(reader);
        if (!root->valid()) {
            return nullptr;
        }
        return MakeChunk(chunkX, chunkZ, root);
    }

protected:
    explicit Chunk(Chunk &s)
        : fChunkX(s.fChunkX)
        , fChunkZ(s.fChunkZ)
        , fDataVersion(s.fDataVersion)
        , fLastUpdate(s.fLastUpdate)
        , fStatus(s.fStatus)
        , fTerrianPopulated(s.fTerrianPopulated)
        , fMinChunkSectionY(s.fMinChunkSectionY)
        , fCreateEmptySection(s.fCreateEmptySection) {
        fSections.swap(s.fSections);
        fBiomes.swap(s.fBiomes);
        fEntities.swap(s.fEntities);
        fTileEntities.swap(s.fTileEntities);
        fStructures.swap(s.fStructures);
        fTileTicks.swap(s.fTileTicks);
        fLiquidTicks.swap(s.fLiquidTicks);
    }

private:
    Chunk(int chunkX, int chunkZ,
          std::vector<std::shared_ptr<ChunkSection>> const &sections,
          int dataVersion,
          std::vector<biomes::BiomeId> &biomes,
          std::vector<std::shared_ptr<nbt::CompoundTag>> &entities,
          std::vector<std::shared_ptr<nbt::CompoundTag>> &tileEntities,
          std::shared_ptr<nbt::CompoundTag> const &structures,
          int64_t lastUpdate,
          std::vector<TickingBlock> tileTicks,
          std::vector<TickingBlock> liquidTicks,
          std::string const &status,
          std::optional<bool> terrianPopulated,
          std::function<std::shared_ptr<ChunkSection>(int sectionY)> createEmptySection)
        : fChunkX(chunkX)
        , fChunkZ(chunkZ)
        , fDataVersion(dataVersion)
        , fStructures(structures)
        , fLastUpdate(lastUpdate)
        , fTileTicks(tileTicks)
        , fLiquidTicks(liquidTicks)
        , fStatus(status)
        , fTerrianPopulated(terrianPopulated)
        , fCreateEmptySection(createEmptySection) {
        if (sections.empty()) {
            fMinChunkSectionY = 0;
        } else {
            std::optional<int> minChunkSectionY;
            std::optional<int> maxChunkSectionY;
            for (auto const &section : sections) {
                int const y = section->y();
                if (minChunkSectionY) {
                    minChunkSectionY = std::min(*minChunkSectionY, y);
                } else {
                    minChunkSectionY = y;
                }
                if (maxChunkSectionY) {
                    maxChunkSectionY = std::max(*maxChunkSectionY, y);
                } else {
                    maxChunkSectionY = y;
                }
            }
            fMinChunkSectionY = *minChunkSectionY;
            fSections.resize(*maxChunkSectionY - *minChunkSectionY + 1);
            for (auto const &section : sections) {
                int const y = section->y();
                int const idx = y - fMinChunkSectionY;
                fSections[idx] = section;
            }
        }
        fBiomes.swap(biomes);
        fEntities.swap(entities);
        for (auto const &item : tileEntities) {
            assert(item);
            auto x = item->int32("x");
            auto y = item->int32("y");
            auto z = item->int32("z");
            if (!x || !y || !z) {
                continue;
            }
            Pos3i pos(*x, *y, *z);
            fTileEntities.insert(std::make_pair(pos, item));
        }
    }

private:
    std::shared_ptr<ChunkSection> unsafeSectionAtBlock(int y) const {
        int const sectionIndex = y / 16;
        int const idx = sectionIndex - fMinChunkSectionY;
        if (idx < 0 || fSections.size() <= idx) {
            return nullptr;
        } else {
            return fSections[idx];
        }
    }

    std::shared_ptr<ChunkSection> sectionAtBlock(int y) {
        int const sectionIndex = y / 16;
        int const idx = sectionIndex - fMinChunkSectionY;
        std::shared_ptr<ChunkSection> section;
        if (0 <= idx && idx < fSections.size()) {
            section = fSections[idx];
        }
        if (!section && 0 <= sectionIndex && sectionIndex < 16 && fCreateEmptySection) {
            section = fCreateEmptySection(sectionIndex);
            if (section) {
                if (sectionIndex < 0 || fSections.size() <= sectionIndex) {
                    int minChunkSectionY = std::min(sectionIndex, fMinChunkSectionY);
                    int maxChunkSectionY = std::max(sectionIndex, fMinChunkSectionY + (int)fSections.size());
                    std::vector<std::shared_ptr<ChunkSection>> sections;
                    sections.resize(maxChunkSectionY - minChunkSectionY + 1);
                    for (auto const &s : fSections) {
                        if (!s) {
                            continue;
                        }
                        sections[s->y() - minChunkSectionY] = s;
                    }
                    fMinChunkSectionY = minChunkSectionY;
                    fSections.swap(sections);
                }
                fSections[sectionIndex - fMinChunkSectionY] = section;
            }
        }
        return section;
    }

    static void ParseBiomes(nbt::Tag const *biomesTag, std::vector<biomes::BiomeId> &result) {
        if (!biomesTag) {
            return;
        }
        if (biomesTag->type() == nbt::Tag::Type::IntArray) {
            std::vector<int32_t> const &value = biomesTag->asIntArray()->value();
            size_t const size = value.size();
            if (size == 256 || size == 1024 || size == 1536) {
                result.resize(size);
                for (int i = 0; i < size; i++) {
                    result[i] = biomes::FromInt(value[i]);
                }
            }
        } else if (biomesTag->type() == nbt::Tag::Type::ByteArray) {
            std::vector<uint8_t> const &value = biomesTag->asByteArray()->value();
            if (value.size() == 256) {
                result.resize(256);
                for (int i = 0; i < 256; i++) {
                    result[i] = biomes::FromInt(value[i]);
                }
            }
        }
    }

public:
    int const fChunkX;
    int const fChunkZ;
    std::vector<std::shared_ptr<ChunkSection>> fSections;
    std::vector<biomes::BiomeId> fBiomes;
    int const fDataVersion;
    std::vector<std::shared_ptr<nbt::CompoundTag>> fEntities;
    std::unordered_map<Pos3i, std::shared_ptr<nbt::CompoundTag>, Pos3iHasher> fTileEntities;
    std::shared_ptr<mcfile::nbt::CompoundTag> fStructures;
    int64_t fLastUpdate;
    std::vector<TickingBlock> fTileTicks;
    std::vector<TickingBlock> fLiquidTicks;

protected:
    std::string const fStatus;
    std::optional<bool> fTerrianPopulated;
    int fMinChunkSectionY;
    std::function<std::shared_ptr<ChunkSection>(int sectionY)> const fCreateEmptySection;
};

} // namespace mcfile::je
