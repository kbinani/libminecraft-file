#pragma once

namespace mcfile::je {

class McaBuilder {
    class OffsetOutputStream : public stream::OutputStream {
    public:
        explicit OffsetOutputStream(stream::OutputStream &base)
            : fBase(base) {
            fOffset = fBase.pos();
        }

        bool write(void const *buffer, size_t size) override {
            return fBase.write(buffer, size);
        }

        bool seek(uint64_t offset) override {
            return fBase.seek(offset + fOffset);
        }

        uint64_t pos() const override {
            uint64_t p = fBase.pos();
            if (p <= fOffset) {
                return 0;
            } else {
                return p - fOffset;
            }
        }

        bool truncate() override {
            return false;
        }

    private:
        stream::OutputStream &fBase;
        uint64_t fOffset;
    };

public:
    explicit McaBuilder(std::filesystem::path const &path) {
        fStream.reset(new stream::FileOutputStream(path));
    }

    bool writeChunk(int localX, int localZ, std::function<bool(stream::OutputStream &out)> callback) {
        if (localX < 0 || 32 <= localX || localZ < 0 || 32 <= localZ) {
            return false;
        }
        uint64_t head = fUsedSectors * 4096;
        uint64_t before = head + 4 + 1;
        if (!fStream->seek(before)) {
            return false;
        }
        OffsetOutputStream stream(*fStream);
        if (!callback(stream)) {
            return false;
        }
        uint64_t after = fStream->pos();
        if (after == before) {
            return true;
        }
        assert(after > before);
        uint32_t sectors = Ceil<uint64_t>(after, 4096) / 4096 - fUsedSectors;
        fUsedSectors += sectors;
        uint64_t fillUntil = fUsedSectors * 4096;
        uint64_t remain = fillUntil - after;
        uint8_t zero[512];
        std::fill_n(zero, 512, 0);
        while (remain > 0) {
            auto amount = (std::min)(remain, (uint64_t)512);
            if (!fStream->write(zero, amount)) {
                return false;
            }
            remain -= amount;
        }
        if (!fStream->seek(head)) {
            return false;
        }
        uint32_t size = U32BEFromNative(after - (head + 4));
        if (!fStream->write(&size, sizeof(size))) {
            return false;
        }
        uint8_t compressType = 2;
        if (!fStream->write(&compressType, sizeof(compressType))) {
            return false;
        }
        int index = localZ * 32 + localX;
        uint32_t loc = U32BEFromNative((uint32_t((head >> 12) << 8) & 0xFFFFFF00) | (sectors & 0xFF));
        if (!fStream->seek(index * 4)) {
            return false;
        }
        if (!fStream->write(&loc, sizeof(loc))) {
            return false;
        }
        if (!fStream->seek(4096 + index * 4)) {
            return false;
        }
        uint32_t time = std::time(nullptr);
        return fStream->write(&time, sizeof(time));
    }

private:
    template<class T>
    static T Ceil(T v, T div) {
        T odd = v % div;
        if (odd == 0) {
            return v;
        } else {
            return v + (div - odd);
        }
    }

private:
    std::unique_ptr<stream::FileOutputStream> fStream;
    int fUsedSectors = 2;
};

} // namespace mcfile::je
