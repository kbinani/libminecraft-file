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

    static std::shared_ptr<WritableChunk> MakeEmpty(int chunkX, int chunkY, int chunkZ) {
        using namespace std;
        using namespace mcfile::nbt;

        auto empty = make_shared<mcfile::je::Chunk>(chunkX, chunkY, chunkZ);
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
        using namespace std;
        using namespace mcfile;
        auto compound = toCompoundTag();
        if (!compound) {
            return false;
        }
        auto stream = make_shared<stream::ByteStream>();
        auto writer = make_shared<stream::OutputStreamWriter>(stream, stream::WriteOption{.fLittleEndian = false});
        if (!compound->writeAsRoot(*writer)) {
            return false;
        }
        std::vector<uint8_t> buffer;
        stream->drain(buffer);
        if (!Compression::compress(buffer)) {
            return false;
        }
        if (!s.write(buffer.data(), buffer.size())) {
            return false;
        }
        return true;
    }

    bool writeEntities(stream::OutputStream &s) const {
        using namespace std;
        using namespace mcfile::nbt;
        using namespace mcfile::stream;
        auto c = make_shared<CompoundTag>();
        auto entities = make_shared<ListTag>(Tag::Type::Compound);
        for (auto const &it : fEntities) {
            entities->push_back(it);
        }
        c->set("Entities", entities);
        auto stream = make_shared<ByteStream>();
        OutputStreamWriter osw(stream, {.fLittleEndian = false});
        if (!c->writeAsRoot(osw)) {
            return false;
        }
        vector<uint8_t> buffer;
        stream->drain(buffer);
        if (!Compression::compress(buffer)) {
            return false;
        }
        if (!s.write(buffer.data(), buffer.size())) {
            return false;
        }
        return true;
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
        };
        auto level = make_shared<CompoundTag>();
        shared_ptr<CompoundTag> existing = fRoot->compoundTag("");
        if (existing) {
            for (auto it : *existing) {
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
                copy(fLegacyBiomes.begin(), fLegacyBiomes.end(), back_inserter(biomes));
                level->set("Biomes", make_shared<IntArrayTag>(biomes));
            }
        } else {
            if (fLegacyBiomes.size() == 256) {
                vector<uint8_t> biomes;
                biomes.reserve(256);
                copy(fLegacyBiomes.begin(), fLegacyBiomes.end(), back_inserter(biomes));
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

public:
    std::shared_ptr<mcfile::nbt::CompoundTag> fRoot;
};
} // namespace mcfile::je
