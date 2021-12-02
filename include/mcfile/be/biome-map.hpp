#pragma once

namespace mcfile::be {
class BiomeMap {
public:
    std::map<int8_t, std::shared_ptr<BiomeSection>> fSections;
};
} // namespace mcfile::be
