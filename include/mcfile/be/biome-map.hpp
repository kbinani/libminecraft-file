#pragma once

namespace mcfile::be {
class BiomeMap {
public:
    BiomeMap(int minChunkY, int maxChunkY)
        : fMinChunkY(minChunkY) {
        for (int y = minChunkY; y <= maxChunkY; y++) {
            fSections.push_back(std::make_shared<BiomeSection>());
        }
    }

    std::optional<std::string> encode() const {
        using namespace std;
        string ret;
        vector<string> sections;

        for (auto const &section : fSections) {
            string encoded;
            if (!section->encode(encoded)) {
                return nullopt;
            }
            sections.push_back(encoded);
        }
        while (sections.size() > 9) {
            int len = sections.size();
            if (sections[len - 1] == sections[len - 2] && sections[len - 2] == sections[len - 3]) {
                sections.pop_back();
            } else {
                break;
            }
        }
        if (sections.size() != fSections.size()) {
            string trailing(16, 0xff);
            sections.push_back(trailing);
        }
        for (string const &s : sections) {
            ret += s;
        }
        return ret;
    }

    void set(int localX, int y, int localZ, biomes::BiomeId biome) {
        auto const &section = this->section(y);
        if (!section) {
            return;
        }
        int chunkY = Coordinate::ChunkFromBlock(y);
        int localY = y - chunkY * 16;
        section->set(localX, localY, localZ, biome);
    }

    size_t numSections() const {
        return fSections.size();
    }

    static std::shared_ptr<BiomeMap> Decode(int minChunkY, std::string const &v, size_t offset) {
        auto ret = std::make_shared<BiomeMap>(minChunkY, minChunkY);
        ret->fSections.clear();

        size_t ptr = offset;
        while (ptr < v.size()) {
            uint8_t format = *(uint8_t *)(v.data() + ptr);
            if (format == 0xff) {
                break;
            }
            auto sec = BiomeSection::Decode(v, &ptr);
            if (!sec) {
                return nullptr;
            }
            ret->fSections.push_back(sec);
        }
        return ret;
    }

private:
    std::shared_ptr<BiomeSection> section(int y) {
        int chunkY = Coordinate::ChunkFromBlock(y);
        int index = chunkY - fMinChunkY;
        if (index < 0 || fSections.size() <= index) {
            return nullptr;
        }
        return fSections[index];
    }

private:
    int const fMinChunkY;
    std::vector<std::shared_ptr<BiomeSection>> fSections;
};
} // namespace mcfile::be
