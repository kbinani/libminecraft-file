#pragma once

namespace mcfile::je::chunksection {

class ChunkSectionGenerator {
public:
    //clang-format: off
    enum : int {
        // :
        //1444 (17w43a)
        //1449 (17w46a)
        kMinDataVersionChunkSection113 = 1451,
        //1451 (17w47a)
        //1453 (17w48a)
        //1457 (17w50a)
        //1459 (18w01a)
        // :
        //2526 (20w16a)
        kMinDataVersionChunkSection116 = 2529,
        //2529 (20w17a)
        //2555 (20w22a)
        // :
        kMinDataVersionChunkSection118 = 2834, // Cave & Cliffs II
        //2834 (21w37a)
        //2835 (21w38a)
        //2839 (21w41a)
        //2840 (21w42a)
        //2844 (21w43a)
        //2847 (1.18pre1)
        //2850 (1.18pre4)
        //2857 (1.18rc2)
        //2858 (1.18rc3)
        // :
    };
    //clang-format: on

    static std::function<std::shared_ptr<ChunkSection>(int sectionY)>
    MakeChunkSections(std::shared_ptr<nbt::ListTag> const &sections,
                      int const dataVersion,
                      int chunkX, int chunkZ,
                      std::vector<std::shared_ptr<nbt::CompoundTag>> const &tileEntities,
                      std::vector<std::shared_ptr<ChunkSection>> &result) {
        using namespace std;

        result.clear();
        if (sections) {
            result.reserve(sections->size());
        }

        if (dataVersion < kMinDataVersionChunkSection113) {
            if (sections) {
                chunksection::ChunkSection112::MakeChunkSections(sections, chunkX, chunkZ, tileEntities, result);
            }
            return chunksection::ChunkSection112::MakeEmpty;
        } else if (dataVersion < kMinDataVersionChunkSection116) {
            if (sections) {
                for (auto const &it : *sections) {
                    auto section = it->asCompound();
                    if (!section) {
                        continue;
                    }
                    auto const &converted = chunksection::ChunkSection113::MakeChunkSection(section);
                    if (converted) {
                        result.push_back(converted);
                    }
                }
            }
            return chunksection::ChunkSection113::MakeEmpty;
        } else if (dataVersion < kMinDataVersionChunkSection118) {
            if (sections) {
                for (auto const &it : *sections) {
                    auto section = it->asCompound();
                    if (!section) {
                        continue;
                    }
                    auto const &converted = chunksection::ChunkSection116::MakeChunkSection(section);
                    if (converted) {
                        result.push_back(converted);
                    }
                }
            }
            return chunksection::ChunkSection116::MakeEmpty;
        } else {
            if (sections) {
                for (auto const &it : *sections) {
                    auto section = it->asCompound();
                    if (!section) {
                        continue;
                    }
                    auto const &converted = chunksection::ChunkSection118::MakeChunkSection(section, dataVersion);
                    if (converted) {
                        result.push_back(converted);
                    }
                }
            }
            return chunksection::ChunkSection118::MakeEmpty;
        }
    }

private:
    ChunkSectionGenerator() = delete;
};

} // namespace mcfile::je::chunksection
