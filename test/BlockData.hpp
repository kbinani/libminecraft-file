#pragma once

class BlockData {
public:
    BlockData(std::string const &name, int blockLight, int skyLight)
        : fName(name)
        , fBlockLight(blockLight)
        , fSkyLight(skyLight) {
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
            if (tokens.size() < 6) {
                continue;
            }
            try {
                int const x = std::stoi(tokens[0]);
                int const y = std::stoi(tokens[1]);
                int const z = std::stoi(tokens[2]);
                std::string const name = tokens[3];
                int const blockLight = std::stoi(tokens[4]);
                int const skyLight = std::stoi(tokens[5]);
                buffer.insert(std::make_pair(MakePoint3D(x, y, z), BlockData(name, blockLight, skyLight)));
            } catch (...) {
            }
        }
    }

public:
    std::string fName;
    int const fBlockLight;
    int const fSkyLight;
};
