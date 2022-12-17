#pragma once

namespace mcfile {

class Compression {
public:
    Compression() = delete;

    template<class Out>
    static bool Compress(void *in, size_t inSize, Out &out, int level = Z_BEST_COMPRESSION) {
        if (inSize == 0) {
            return true;
        }
        auto compressor = libdeflate_alloc_compressor(level);
        size_t bound = libdeflate_zlib_compress_bound(compressor, inSize);
        out.resize(bound, 0);
        size_t size = libdeflate_zlib_compress(compressor, in, inSize, out.data(), out.size());
        libdeflate_free_compressor(compressor);
        if (size == 0) {
            return false;
        }
        out.resize(size);
        return true;
    }

    static bool Compress(std::vector<uint8_t> &inout, int level = Z_BEST_COMPRESSION) {
        if (inout.empty()) {
            return true;
        }
        auto compressor = libdeflate_alloc_compressor(level);
        size_t bound = libdeflate_zlib_compress_bound(compressor, inout.size());
        std::vector<uint8_t> buff(bound, 0);
        size_t size = libdeflate_zlib_compress(compressor, inout.data(), inout.size(), buff.data(), buff.size());
        libdeflate_free_compressor(compressor);
        if (size == 0) {
            return false;
        }
        buff.resize(size);
        inout.swap(buff);
        return true;
    }

    static bool Decompress(std::vector<uint8_t> &inout) {
        if (inout.empty()) {
            return true;
        }
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
