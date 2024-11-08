#pragma once

namespace mcfile::blocks {

std::shared_ptr<BlockData> BlockData::Make(mcfile::je::Block const &j) {
    if (j.fName.ends_with(u8"stairs")) {
        return std::make_shared<data::type::Stairs>(j);
    } else {
        return std::shared_ptr<BlockData>(new BlockData(j));
    }
}

std::shared_ptr<BlockData> BlockData::Make(mcfile::be::Block const &b) {
    if (b.fName.ends_with(u8"stairs")) {
        return std::make_shared<data::type::Stairs>(b);
    } else {
        return std::shared_ptr<BlockData>(new BlockData(b));
    }
}

} // namespace mcfile::blocks
