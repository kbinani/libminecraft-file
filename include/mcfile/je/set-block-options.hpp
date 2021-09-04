#pragma once

namespace mcfile::je {

struct SetBlockOptions {
    SetBlockOptions()
        : fRemoveTileEntity(true) {}

    bool fRemoveTileEntity : 1;
};

} // namespace mcfile::je
