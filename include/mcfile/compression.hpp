#pragma once

namespace mcfile {

class Compression {
public:
    Compression() = delete;

    static bool Compress(std::vector<uint8_t> &inout, int level = Z_BEST_COMPRESSION) {
        z_stream zs;
        std::vector<uint8_t> buff(kSegSize, 0);
        std::vector<uint8_t> outData;
        unsigned long prevOut = 0;

        memset(&zs, 0, sizeof(zs));
        if (deflateInit(&zs, level) != Z_OK) {
            return false;
        }
        zs.next_in = inout.data();
        zs.avail_in = inout.size();

        int ret;
        do {
            zs.next_out = buff.data();
            zs.avail_out = buff.size();

            ret = deflate(&zs, Z_FINISH);
            std::copy_n(buff.begin(), zs.total_out - prevOut, std::back_inserter(outData));
            prevOut = zs.total_out;
        } while (ret == Z_OK);

        int r = deflateEnd(&zs);
        if (ret != Z_STREAM_END) {
            return false;
        }
        if (r != Z_OK) {
            return false;
        }

        inout.swap(outData);
        return true;
    }

    static bool Decompress(std::vector<uint8_t> &inout) {
        int ret;
        z_stream zs;
        std::vector<uint8_t> buff(kSegSize, 0);
        std::vector<uint8_t> outData;
        unsigned long prevOut = 0;

        memset(&zs, 0, sizeof(zs));
        if (inflateInit(&zs) != Z_OK) {
            return false;
        }

        zs.next_in = inout.data();
        zs.avail_in = inout.size();

        do {
            zs.next_out = buff.data();
            zs.avail_out = buff.size();

            ret = inflate(&zs, 0);
            std::copy_n(buff.begin(), zs.total_out - prevOut, std::back_inserter(outData));
            prevOut = zs.total_out;
        } while (ret == Z_OK);

        inflateEnd(&zs);
        if (ret != Z_STREAM_END) {
            return false;
        }

        inout.swap(outData);
        return true;
    }

private:
    static constexpr unsigned int kSegSize = 16384;
};

} // namespace mcfile
