#pragma once

namespace mcfile::blocks::data {

class Ageable {
public:
    virtual ~Ageable() {}
    virtual int age() const = 0;
};

} // namespace mcfile::blocks::data
