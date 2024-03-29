#pragma once

namespace mcfile::be {

class DbInterface {
public:
    virtual ~DbInterface() = default;
    virtual std::optional<std::string> get(std::string const &key) = 0;
};

} // namespace mcfile::be
