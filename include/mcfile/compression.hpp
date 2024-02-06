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
        return DecompressDeflateImpl(in, inSize, out, outSizeExact, libdeflate_zlib_decompress);
    }

    template<class Out>
    static bool DecompressDeflate(void *in, size_t inSize, Out &out, size_t outSizeExact) {
        return DecompressDeflateImpl(in, inSize, out, outSizeExact, libdeflate_deflate_decompress);
    }

    template<class Out>
    static bool DecompressDeflate(void *in, size_t inSize, std::vector<uint8_t> &out) {
        return DecompressImpl<-15>(in, inSize, out);
    }

    static bool DecompressZlib(std::vector<uint8_t> &inout) {
        std::vector<uint8_t> out;
        if (!DecompressImpl<15>(inout.data(), inout.size(), out)) {
            return false;
        }
        inout.swap(out);
        return true;
    }

    static bool DecompressLz4(std::vector<uint8_t> &inout) {
        using namespace std;
        size_t remaining = inout.size();
        vector<uint8_t> out;
        char magic[] = "LZ4Block";
        size_t offset = 0;
        size_t constexpr headerLength = 21;
        while (remaining > 0) {
            if (remaining < headerLength) {
                return false;
            }
            for (int i = 0; i < 8; i++) {
                if (inout[offset + i] != magic[i]) {
                    return false;
                }
            }
            uint8_t token = inout[offset + 8];
            uint8_t method = token & 0xf0;
            uint32_t compressedLength = U32FromLE(Mem::Read<uint32_t>(inout, offset + 9));
            size_t advance = compressedLength + headerLength;
            if (compressedLength == 0) {
                break;
            } else if (advance >= remaining) {
                return false;
            }
            uint32_t decompressedLength = U32FromLE(Mem::Read<uint32_t>(inout, offset + 13));
            if (decompressedLength == 0) {
                break;
            }
            uint32_t actualChecksum = Mem::Read<uint32_t>(inout, offset + 17);
            if (method == 0x10) {
                // raw
                uint32_t expectedChecksum = XXHash<uint32_t>::Digest(inout.data() + offset + 21, compressedLength, 0x9747b28c) & 0xfffffff;
                if (actualChecksum != expectedChecksum) {
                    return false;
                }
                out.reserve(out.size() + compressedLength);
                copy_n(inout.begin() + offset + headerLength, compressedLength, back_inserter(out));
            } else if (method == 0x20) {
                // compressed
                size_t prev = out.size();
                out.resize(prev + decompressedLength);
                if (LZ4_decompress_safe((char const *)inout.data() + offset + headerLength, (char *)out.data() + prev, compressedLength, decompressedLength) < 0) {
                    return false;
                }
                uint32_t expectedChecksum = XXHash<uint32_t>::Digest(out.data() + prev, decompressedLength, 0x9747b28c) & 0xfffffff;
                if (actualChecksum != expectedChecksum) {
                    return false;
                }
            } else {
                return false;
            }
            offset += advance;
            remaining -= advance;
        }
        inout.swap(out);
        return true;
    }

private:
    template<int WindowSize, class Out>
    static bool DecompressImpl(void *in, size_t inSize, Out &out) {
        out.clear();
        if (inSize == 0) {
            return true;
        }
        int ret;
        z_stream zs;
        if (inSize > std::numeric_limits<decltype(zs.avail_in)>::max()) {
            return false;
        }

        std::vector<uint8_t> buff(kSegSize, 0);
        unsigned long prevOut = 0;

        memset(&zs, 0, sizeof(zs));
        if (inflateInit2(&zs, WindowSize) != Z_OK) {
            return false;
        }

        zs.next_in = (Bytef *)in;
        zs.avail_in = inSize;

        do {
            zs.next_out = buff.data();
            zs.avail_out = buff.size();

            ret = inflate(&zs, 0);
            std::copy_n(buff.begin(), zs.total_out - prevOut, std::back_inserter(out));
            prevOut = zs.total_out;
        } while (ret == Z_OK);

        inflateEnd(&zs);
        if (ret != Z_STREAM_END) {
            return false;
        }

        return true;
    }

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
    static bool DecompressDeflateImpl(void *in, size_t inSize, Out &out, size_t outSize, Decompress decompress) {
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
