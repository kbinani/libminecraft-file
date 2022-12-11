#pragma once

#include "Point3D.hpp"

#include <fstream>
#include <map>
#include <sstream>
#include <string>

class BlockData {
public:
    explicit BlockData(std::string const &name)
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
            std::vector<std::string> tokens;
            for (std::string token; std::getline(ss, token, ',');) {
                tokens.push_back(token);
            }
            if (tokens.size() < 4) {
                continue;
            }
            try {
                int const x = std::stoi(tokens[0]);
                int const y = std::stoi(tokens[1]);
                int const z = std::stoi(tokens[2]);
                std::string const name = tokens[3];
                buffer.insert(std::make_pair(MakePoint3D(x, y, z), BlockData(name)));
            } catch (...) {
            }
        }
    }

public:
    std::string fName;
};
