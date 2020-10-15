#pragma once

namespace mcfile {
namespace stream {

class FileOutputStream : public OutputStream {
public:
    explicit FileOutputStream(std::string const &filePath)
        : fFile(nullptr) {
        FILE *fp = fopen(filePath.c_str(), "wb");

        if (!fp) {
            return;
        }
        this->fFile = fp;
    }

    ~FileOutputStream() {
        if (fFile) {
            fclose(fFile);
        }
    }

    FileOutputStream(FileOutputStream const &) = delete;

    FileOutputStream &operator=(FileOutputStream const &) = delete;

private:
    FILE *fFile;
};

} // namespace stream
} // namespace mcfile
