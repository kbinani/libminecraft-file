#pragma once

namespace mcfile {
namespace detail {

class String {
public:
    String() = delete;

    static std::vector<std::string> Split(std::string const &sentence, char delimiter) {
        std::istringstream input(sentence);
        std::vector<std::string> tokens;
        for (std::string line; std::getline(input, line, delimiter);) {
            tokens.push_back(line);
        }
        return tokens;
    }
};

} // namespace mcfile
} // namespace detail
