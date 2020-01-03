#pragma once

namespace mcfile {
namespace detail {

class Compression {
public:
    Compression() = delete;

    static bool compress(std::vector<uint8_t> &inout) {
        z_stream zs;
        char buff[kSegSize];
        std::vector<uint8_t> outData;

        memset(&zs, 0, sizeof(zs));
        if (deflateInit(&zs, Z_BEST_COMPRESSION) != Z_OK) {
            return false;
        }
        zs.next_in = (Bytef *) inout.data();
        zs.avail_in = inout.size();

        int ret;
        do {
            zs.next_out = reinterpret_cast<Bytef *>(buff);
            zs.avail_out = kSegSize;

            ret = deflate(&zs, Z_FINISH);
            outData.insert(outData.end(), buff, buff + zs.total_out);
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

    static bool decompress(std::vector<uint8_t> &inout) {
        int ret;
        z_stream zs;
        char buff[kSegSize];
        std::vector<uint8_t> outData;
        unsigned long prevOut = 0;

        memset(&zs, 0, sizeof(zs));
        if (inflateInit(&zs) != Z_OK) {
            return false;
        }

        zs.next_in = (Bytef *) inout.data();
        zs.avail_in = inout.size();

        do {
            zs.next_out = reinterpret_cast<Bytef *>(buff);
            zs.avail_out = kSegSize;

            ret = inflate(&zs, 0);
            outData.insert(outData.end(), buff, buff + (zs.total_out - prevOut));
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
    static const unsigned int kSegSize = 16384;
};

} // namespace mcfile
} // namespace detail
