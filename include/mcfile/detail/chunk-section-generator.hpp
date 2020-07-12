#pragma once

namespace mcfile {
namespace detail {

class ChunkSectionGenerator {
public:
    static std::shared_ptr<ChunkSection> MakeChunkSection(nbt::CompoundTag const* section, int const dataVersion) {
        if (dataVersion <= 1449) {
            //1444 (17w43a)
            //1449 (17w46a)
            return detail::ChunkSection_1_12::MakeChunkSection(section);
        } else if (dataVersion <= 2526) {
            //1451 (17w47a)
            //1453 (17w48a)
            //1457 (17w50a)
            //1459 (18w01a)
            
            //2526 (20w16a)
            return detail::ChunkSection_1_13::MakeChunkSection(section);
        } else {
            //2529 (20w17a)
            //2555 (20w22a)
            return detail::ChunkSection_1_16::MakeChunkSection(section);
        }
    }

private:
    ChunkSectionGenerator() = delete;
};

} // namespace detail
} // namespace mcfile
