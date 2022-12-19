#pragma once

namespace mcfile::blocks {

class BlockData {
public:
    virtual ~BlockData() {}

    std::string_view name() const {
        if (fJava) {
            return fJava->fName;
        } else {
            return fBedrock->fName;
        }
    }

    static inline std::shared_ptr<BlockData> Make(mcfile::je::Block const &j);
    static inline std::shared_ptr<BlockData> Make(mcfile::be::Block const &b);

protected:
    explicit BlockData(mcfile::je::Block const &j)
        : fJava(j.copy()) {}

    explicit BlockData(mcfile::be::Block const &b)
        : fBedrock(std::make_shared<mcfile::be::Block const>(b.fName, b.fStates->copy(), b.fVersion)) {}

    std::shared_ptr<mcfile::je::Block const> fJava;
    std::shared_ptr<mcfile::be::Block const> fBedrock;
};

} // namespace mcfile::blocks
