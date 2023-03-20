#pragma once

namespace mcfile {

class Compression {
public:
    Compression() = delete;

    template<class Out>
    static bool CompressDeflate(void *in, size_t inSize, Out &out, int level = Z_BEST_COMPRESSION) {
        return CompressImpl(in, inSize, out, libdeflate_deflate_compress_bound, libdeflate_deflate_compress, level);
    }

    static bool CompressDeflate(std::vector<uint8_t> &inout, int level = Z_BEST_COMPRESSION) {
        return CompressImpl(inout, libdeflate_deflate_compress_bound, libdeflate_deflate_compress, level);
    }

    template<class Out>
    static bool CompressZlib(void *in, size_t inSize, Out &out, int level = Z_BEST_COMPRESSION) {
        return CompressImpl(in, inSize, out, libdeflate_zlib_compress_bound, libdeflate_zlib_compress, level);
    }

    static bool CompressZlib(std::vector<uint8_t> &inout, int level = Z_BEST_COMPRESSION) {
        return CompressImpl(inout, libdeflate_zlib_compress_bound, libdeflate_zlib_compress, level);
    }

    template<class Out>
    static bool DecompressZlib(void *in, size_t inSize, Out &out, size_t outSizeExact) {
        return DecompressImpl(in, inSize, out, outSizeExact, libdeflate_zlib_decompress);
    }

    template<class Out>
    static bool DecompressDeflate(void *in, size_t inSize, Out &out, size_t outSizeExact) {
        return DecompressImpl(in, inSize, out, outSizeExact, libdeflate_deflate_decompress);
    }

    static bool DecompressZlib(std::vector<uint8_t> &inout) {
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
    template<class GetBound, class Compress, class Out>
    static bool CompressImpl(void *in, size_t inSize, Out &out, GetBound getBound, Compress compress, int level = Z_BEST_COMPRESSION) {
        if (inSize == 0) {
            return true;
        }
        auto compressor = libdeflate_alloc_compressor(level);
        size_t bound = getBound(compressor, inSize);
        out.resize(bound, 0);
        size_t size = compress(compressor, in, inSize, out.data(), out.size());
        libdeflate_free_compressor(compressor);
        if (size == 0) {
            return false;
        }
        out.resize(size);
        return true;
    }

    template<class GetBound, class Compress>
    static bool CompressImpl(std::vector<uint8_t> &inout, GetBound getBound, Compress compress, int level = Z_BEST_COMPRESSION) {
        if (inout.empty()) {
            return true;
        }
        auto compressor = libdeflate_alloc_compressor(level);
        size_t bound = getBound(compressor, inout.size());
        std::vector<uint8_t> buff(bound, 0);
        size_t size = compress(compressor, inout.data(), inout.size(), buff.data(), buff.size());
        libdeflate_free_compressor(compressor);
        if (size == 0) {
            return false;
        }
        buff.resize(size);
        inout.swap(buff);
        return true;
    }

    template<class Decompress, class Out>
    static bool DecompressImpl(void *in, size_t inSize, Out &out, size_t outSize, Decompress decompress) {
        if (inSize == 0) {
            return true;
        }
        auto decompressor = libdeflate_alloc_decompressor();
        out.resize(outSize, 0);
        auto ret = decompress(decompressor, in, inSize, out.data(), outSize, nullptr);
        libdeflate_free_decompressor(decompressor);
        if (ret != LIBDEFLATE_SUCCESS) {
            return false;
        }
        return true;
    }

private:
    static constexpr unsigned int kSegSize = 16384;
};

} // namespace mcfile
