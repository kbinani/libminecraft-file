#pragma once

namespace mcfile::stream {

class DeferOpeningOutputStream : public OutputStream {
public:
    using Opener = std::function<std::shared_ptr<OutputStream>()>;

    explicit DeferOpeningOutputStream(Opener opener)
        : fOpener(opener) {}

    bool write(void const *buffer, size_t size) override {
        auto stream = ensureOpened();
        if (stream) {
            return stream->write(buffer, size);
        } else {
            return false;
        }
    }

    bool seek(uint64_t offset) override {
        auto stream = ensureOpened();
        if (stream) {
            return stream->seek(offset);
        } else {
            return false;
        }
    }

    uint64_t pos() const override {
        if (fOpened) {
            return fOpened->pos();
        } else {
            return 0;
        }
    }

    bool truncate() override {
        auto stream = ensureOpened();
        if (stream) {
            return stream->truncate();
        } else {
            return false;
        }
    }

private:
    OutputStream *ensureOpened() {
        if (fOpened) {
            return fOpened.get();
        }
        if (fOpener) {
            fOpened = fOpener();
            Opener().swap(fOpener);
        }
        return fOpened.get();
    }

private:
    Opener fOpener;
    std::shared_ptr<OutputStream> fOpened;
};

} // namespace mcfile::stream
