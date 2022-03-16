#pragma once

namespace mcfile::blocks::data {

class Bisected {
public:
    enum class Half {
        Bottom,
        Top,
    };

    virtual ~Bisected() {}
    virtual Half half() const = 0;
};

} // namespace mcfile::blocks::data
