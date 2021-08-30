#pragma once

namespace mcfile::je {

class WritableChunk : public Chunk {
public:
    static std::shared_ptr<WritableChunk> MakeChunk(int chunkX, int chunkZ, std::shared_ptr<nbt::CompoundTag> const& root) {
        auto chunk = Chunk::MakeChunk(chunkX, chunkZ, root);
        auto ret = std::shared_ptr<WritableChunk>(new WritableChunk(*chunk, root));
    }

    std::shared_ptr<nbt::CompoundTag> toCompoundTag() const {
        using namespace std;
        using namespace mcfile::nbt;
        auto root = make_shared<CompoundTag>();
        root->set("DataVersion", make_shared<IntTag>(fDataVersion));

        auto level = make_shared<CompoundTag>();

        level->set("xPos", make_shared<IntTag>(fChunkX));
        level->set("zPos", make_shared<IntTag>(fChunkZ));

        vector<shared_ptr<ChunkSection>> sections;
        for (auto const& section : fSections) {
            if (section) {
                sections.push_back(section);
            }
        }
        if (!sections.empty()) {
            sort(sections.begin(), sections.end(), [](auto const& a, auto const& b) {
                return a->rawY() < b->rawY();
                });
            auto sectionsList = make_shared<ListTag>(Tag::Type::Compound);
            for (auto const& section : sections) {
                auto s = section->toCompoundTag();
                if (!s) {
                    return nullptr;
                }
                sectionsList->push_back(s);
            }
            level->set("Sections", sectionsList);
        }

        std::vector<int32_t> biomes;
        for (biomes::BiomeId biome : fBiomes) {
            biomes.push_back(static_cast<int32_t>(biome));
        }
        level->set("Biomes", make_shared<IntArrayTag>(biomes));

        auto entities = make_shared<ListTag>(Tag::Type::Compound);
        for (auto const& entity : fEntities) {
            entities->push_back(entity->clone());
        }
        level->set("Entities", entities);

        auto tileEntities = make_shared<ListTag>(Tag::Type::Compound);
        for (auto const& it : fTileEntities) {
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
        CompoundTag const* existingLevel = fRoot->query("/Level")->asCompound();
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

    bool write(stream::OutputStream &s) const {
        using namespace std;
        using namespace mcfile;
        auto compound = toCompoundTag();
        if (!compound) {
            return false;
        }
        auto stream = make_shared<stream::ByteStream>();
        auto writer = make_shared<stream::OutputStreamWriter>(stream, stream::WriteOption{.fLittleEndian = false});
        auto root = make_shared<nbt::CompoundTag>();
        root->set("", compound);
        root->write(*writer);
        std::vector<uint8_t> buffer;
        stream->drain(buffer);
        Compression::compress(buffer);
        s.write(buffer.data(), buffer.size());
        return true;
    }

private:
    WritableChunk(Chunk &s, std::shared_ptr<nbt::CompoundTag> const& root)
        : Chunk(s)
        , fRoot(root)
    {
    }

public:
    std::shared_ptr<mcfile::nbt::CompoundTag> fRoot;
};
}
