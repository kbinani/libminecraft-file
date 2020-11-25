#pragma once

namespace mcfile {
namespace detail {

class ChunkSectionGenerator {
public:
    static void MakeChunkSections(std::shared_ptr<nbt::ListTag> const& sections, int const dataVersion, int chunkX, int chunkZ, std::vector<std::shared_ptr<nbt::CompoundTag>> const& tileEntities, std::vector<std::shared_ptr<ChunkSection>> &result) {
        using namespace std;

        result.clear();
        if (!sections) {
            return;
        }
        result.reserve(sections->size());

        if (dataVersion <= 1449) {
            //1444 (17w43a)
            //1449 (17w46a)

            detail::ChunkSection_1_12::MakeChunkSections(sections, chunkX, chunkZ, tileEntities, result);
        } else if (dataVersion <= 2526) {
            //1451 (17w47a)
            //1453 (17w48a)
            //1457 (17w50a)
            //1459 (18w01a)

            //2526 (20w16a)
            for (auto const& it : *sections) {
                auto section = it->asCompound();
                if (!section) {
                    continue;
                }
                auto const& converted = detail::ChunkSection_1_13::MakeChunkSection(section);
                if (converted) {
                    result.push_back(converted);
                }
            }
        } else {
            //2529 (20w17a)
            //2555 (20w22a)
            for (auto const& it : *sections) {
                auto section = it->asCompound();
                if (!section) {
                    continue;
                }
                auto const& converted = detail::ChunkSection_1_16::MakeChunkSection(section);
                if (converted) {
                    result.push_back(converted);
                }
            }
        }
    }

private:
    ChunkSectionGenerator() = delete;
};

} // namespace detail
} // namespace mcfile
