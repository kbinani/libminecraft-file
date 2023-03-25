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
        std::basic_ifstream<char8_t> stream(file);
        for (std::u8string line; std::getline(stream, line, u8'\n');) {
            if (line.size() > 0 && line[0] == u8'#') {
                continue;
            }
            std::basic_istringstream<char8_t> ss(line);
            std::vector<std::u8string> tokens;
            for (std::u8string token; std::getline(ss, token, u8',');) {
                tokens.push_back(token);
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
