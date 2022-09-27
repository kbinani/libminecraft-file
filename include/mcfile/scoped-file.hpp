#pragma once

namespace mcfile {

class ScopedFile {
public:
    explicit ScopedFile(FILE *file)
        : fFile(file) {
    }

    ScopedFile()
        : fFile(nullptr) {}

    ScopedFile(ScopedFile const &) = delete;
    ScopedFile &operator=(ScopedFile const &) = delete;

    ~ScopedFile() {
        close();
    }

    void close() {
        if (fFile) {
            fclose(fFile);
            fFile = nullptr;
        }
    }

    FILE *get() const {
        return fFile;
    }

    operator bool() const {
        return fFile != nullptr;
    }

private:
    FILE *fFile;
};

} // namespace mcfile
