#pragma once

namespace mcfile {

class Chunk {
public:
    std::shared_ptr<Block const> blockAt(int x, int y, int z) const {
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

    int blockLightAt(int x, int y, int z) const {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return 0;
        }
        if (y < 0 || 256 <= y) {
            return -1;
        }
        int const sectionY = y / 16;
        auto const& section = fSections[sectionY];
        if (!section) {
            return -1;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        int const offsetY = y - sectionY * 16;
        return section->blockLightAt(offsetX, offsetY, offsetZ);
    }

    int skyLightAt(int x, int y, int z) const {
        int const chunkX = Coordinate::ChunkFromBlock(x);
        int const chunkZ = Coordinate::ChunkFromBlock(z);
        if (chunkX != fChunkX || chunkZ != fChunkZ) {
            return 0;
        }
        if (y < 0 || 256 <= y) {
            return -1;
        }
        int const sectionY = y / 16;
        auto const& section = fSections[sectionY];
        if (!section) {
            return -1;
        }
        int const offsetX = x - chunkX * 16;
        int const offsetZ = z - chunkZ * 16;
        int const offsetY = y - sectionY * 16;
        return section->skyLightAt(offsetX, offsetY, offsetZ);
    }

    biomes::BiomeId biomeAt(int x, int y, int z) const {
        if (fDataVersion >= 2203) { // 19w36a
            int const offsetX = (x - fChunkX * 16) / 4;
            int const offsetY = y / 4;
            int const offsetZ = (z - fChunkZ * 16) / 4;
            if (offsetX < 0 || 4 <= offsetX || offsetZ < 0 || 4 <= offsetZ || offsetY < 0 || 64 <= offsetY) {
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

    biomes::BiomeId biomeAt(int x, int z) const {
        return biomeAt(x, 0, z);
    }

    int minBlockX() const { return fChunkX * 16; }
    int maxBlockX() const { return fChunkX * 16 + 15; }

    int minBlockZ() const { return fChunkZ * 16; }
    int maxBlockZ() const { return fChunkZ * 16 + 15; }

    static std::shared_ptr<Chunk> MakeChunk(int chunkX, int chunkZ, nbt::CompoundTag const& root) {
        using namespace std;

        auto level = root.query("/Level")->asCompound();
        if (!level) {
            return nullptr;
        }

        int dataVersion = 0;
        auto dataVersionTag = root.query("/DataVersion")->asInt();
        if (dataVersionTag) {
            // *.mca created by Minecraft 1.2.1 does not have /DataVersion tag
            dataVersion = dataVersionTag->fValue;
        }
        auto sectionsTag = level->listTag("Sections");
        if (!sectionsTag) {
            return nullptr;
        }
        vector<shared_ptr<ChunkSection>> sections;
        for (auto sectionTag : *sectionsTag) {
            auto section = detail::ChunkSectionGenerator::MakeChunkSection(sectionTag->asCompound(), dataVersion);
            if (section) {
                sections.push_back(section);
            }
        }

        vector<biomes::BiomeId> biomes;
        auto biomesTag = level->query("Biomes");
        ParseBiomes(biomesTag, biomes);

        vector<shared_ptr<nbt:: CompoundTag>> entities;
        auto entitiesTag = level->listTag("Entities");
        if (entitiesTag && entitiesTag->fType == nbt::Tag::TAG_Compound) {
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

        vector<shared_ptr<nbt::CompoundTag>> tileEntities;
        auto tileEntitiesList = level->listTag("TileEntities");
        if (tileEntitiesList && tileEntitiesList->fType == nbt::Tag::TAG_Compound) {
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

        string s;
        auto status = level->stringTag("Status");
        if (status) {
            s = status->fValue;
        }

        auto structures = level->compoundTag("Structures");

        return std::shared_ptr<Chunk>(new Chunk(chunkX, chunkZ, sections, dataVersion, biomes, entities, tileEntities, s, structures));
    }

    static std::shared_ptr<Chunk> LoadFromCompressedChunkNbtFile(std::string const& filePath, int chunkX, int chunkZ) {
        auto stream = std::make_shared<mcfile::stream::FileInputStream>(filePath);
        if (!stream->valid()) {
            return nullptr;
        }
        std::vector<uint8_t> buffer(stream->length());
        if (!stream->read(buffer.data(), 1, buffer.size())) {
            return nullptr;
        }
        if (!detail::Compression::decompress(buffer)) {
            return nullptr;
        }
        auto root = std::make_shared<mcfile::nbt::CompoundTag>();
        auto bs = std::make_shared<mcfile::stream::ByteStream>(buffer);
        stream::InputStreamReader reader(bs);
        root->read(reader);
        if (!root->valid()) {
            return nullptr;
        }
        return MakeChunk(chunkX, chunkZ, *root);
    }
    
private:
    explicit Chunk(int chunkX, int chunkZ, std::vector<std::shared_ptr<ChunkSection>> const& sections, int dataVersion, std::vector<biomes::BiomeId> & biomes, std::vector<std::shared_ptr<nbt::CompoundTag>> &entities, std::vector<std::shared_ptr<nbt::CompoundTag>>& tileEntities, std::string const& status, std::shared_ptr<nbt::CompoundTag> const& structures)
        : fChunkX(chunkX)
        , fChunkZ(chunkZ)
        , fSections(16, nullptr)
        , fDataVersion(dataVersion)
        , fStatus(status)
        , fStructures(structures)
    {
        for (auto section : sections) {
            int const y = section->y();
            if (0 <= y && y < 16) {
                fSections[y] = section;
            }
        }
        fBiomes.swap(biomes);
        fEntities.swap(entities);
        fTileEntities.swap(tileEntities);
    }

    static void ParseBiomes(nbt::Tag const* biomesTag, std::vector<biomes::BiomeId> &result) {
        if (!biomesTag) {
            return;
        }
        if (biomesTag->id() == nbt::Tag::TAG_Int_Array) {
            std::vector<int32_t> const& value = biomesTag->asIntArray()->value();
            size_t const size = value.size();
            if (size == 256 || size == 1024) {
                result.resize(size);
                for (int i = 0; i < size; i++) {
                    result[i] = biomes::FromInt(value[i]);
                }
            }
        } else if (biomesTag->id() == nbt::Tag::TAG_Byte_Array) {
            std::vector<uint8_t> const& value = biomesTag->asByteArray()->value();
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
    std::vector<std::shared_ptr<nbt::CompoundTag>> fTileEntities;
    std::string const fStatus;
    std::shared_ptr<mcfile::nbt::CompoundTag> fStructures;
};

} // namespace mcfile
