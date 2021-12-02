#pragma once

namespace mcfile::be {
class BiomeMap {
public:
    explicit BiomeMap(int minChunkY)
        : fMinChunkY(minChunkY) {
    }

    std::optional<std::string> encode() const {
        using namespace std;
        if (fSections.empty()) {
            return nullopt;
        }
        string ret;
        for (int chunkY = fMinChunkY; chunkY < fMinChunkY + fSections.size(); chunkY++) {
            auto const &section = fSections.at(chunkY);
            string encoded;
            if (!section->encode(encoded)) {
                return nullopt;
            }
            ret += encoded;
        }
        return ret;
    }

    void set(int localX, int y, int localZ, biomes::BiomeId biome) {
        auto const &section = this->section(y);
        int chunkY = Coordinate::ChunkFromBlock(y);
        int localY = y - chunkY * 16;
        section->set(localX, localY, localZ, biome);
    }

private:
    std::shared_ptr<BiomeSection> section(int y) {
        int chunkY = Coordinate::ChunkFromBlock(y);
        for (int y = fMinChunkY; y <= chunkY; y++) {
            if (!fSections[y]) {
                fSections[y].reset(new BiomeSection);
            }
        }
        return fSections[chunkY];
    }

private:
    int const fMinChunkY;
    std::map<int8_t, std::shared_ptr<BiomeSection>> fSections;
};
} // namespace mcfile::be
