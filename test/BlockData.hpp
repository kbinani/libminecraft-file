#pragma once

#include "Point3D.hpp"

#include <map>
#include <sstream>
#include <string>

class BlockData {
public:
    explicit BlockData(std::u8string const &name)
        : fName(name) {
    }

    BlockData() = delete;

    static void ReadAll(std::string const &file, std::map<Point3D, BlockData> &buffer) {
        buffer.clear();
        std::ifstream stream(file);
        for (std::string line; std::getline(stream, line, '\n');) {
            if (line.size() > 0 && line[0] == '#') {
                continue;
            }
            std::istringstream ss(line);
            std::vector<std::u8string> tokens;
            for (std::string token; std::getline(ss, token, ',');) {
                std::u8string u8token;
                u8token.assign((char8_t const *)token.data(), token.size());
                tokens.push_back(u8token);
            }
            if (tokens.size() < 4) {
                continue;
            }
            try {
                int const x = mcfile::String::Toi<int>(tokens[0]);
                int const y = mcfile::String::Toi<int>(tokens[1]);
                int const z = mcfile::String::Toi<int>(tokens[2]);
                std::u8string const name = tokens[3];
                buffer.insert(std::make_pair(MakePoint3D(x, y, z), BlockData(name)));
            } catch (...) {
            }
        }
    }

public:
    std::u8string fName;
};
