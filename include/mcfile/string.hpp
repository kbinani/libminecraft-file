#pragma once

namespace mcfile {

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

    static bool EndsWith(std::string const &s, std::string const &search) {
        if (s.size() < search.size()) {
            return false;
        }
        if (search.empty()) {
            return false;
        }
        auto idx = s.rfind(search);
        if (idx == std::string::npos) {
            return false;
        }
        return idx + search.size() == s.size();
    }
};

} // namespace mcfile
