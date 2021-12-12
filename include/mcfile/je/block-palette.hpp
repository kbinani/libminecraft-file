#pragma once

namespace mcfile::je {

class BlockHasher {
public:
    size_t operator()(std::shared_ptr<Block const> const &k) const {
        std::string key;
        if (k) {
            key = k->toString();
        }
        return std::hash<std::string>()(key);
    }
};

class BlockPred {
public:
    bool operator()(std::shared_ptr<Block const> const &a, std::shared_ptr<Block const> const &b) const {
        if (!a && !b) {
            return true;
        }
        if (!a || !b) {
            return false;
        }
        if (a.get() == b.get()) {
            return true;
        }
        return a->toString() == b->toString();
    }
};

class BlockPalette : public PaletteList<std::shared_ptr<Block const>, uint16_t, 4096, BlockHasher, BlockPred> {};

} // namespace mcfile::je
