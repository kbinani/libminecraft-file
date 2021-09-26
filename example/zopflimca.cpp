#include <ThreadPool.h>
#include <minecraft-file.hpp>

#include <future>
#include <iostream>

namespace {

std::optional<std::filesystem::path> CreateTempDir(std::filesystem::path const &tempDir) {
    namespace fs = std::filesystem;
    auto tmp = fs::temp_directory_path();
#if defined(_MSC_VER)
    wchar_t *dir = _wtempnam(tmp.native().c_str(), L"mcfile-tmp-");
    if (dir) {
        fs::path ret(dir);
        fs::create_directory(ret);
        free(dir);
        return ret;
    } else {
        return std::nullopt;
    }
#else
    using namespace std;
    string tmpl("mcfile-tmp-XXXXXX");
    vector<char> buffer;
    copy(tmpl.begin(), tmpl.end(), back_inserter(buffer));
    buffer.push_back(0);
    char *dir = mkdtemp(buffer.data());
    if (dir) {
        return string(dir, strlen(dir));
    } else {
        return nullopt;
    }
#endif
}

} // namespace

int main(int argc, char *argv[]) {
    using namespace std;
    using namespace mcfile;
    using namespace mcfile::je;
    namespace fs = std::filesystem;

    auto temp = CreateTempDir(fs::temp_directory_path());
    if (!temp) {
        cerr << "Error: cannot create temp directory" << endl;
        return 1;
    }

    fs::path directory("./");
    if (argc > 1) {
        directory = fs::path(argv[1]);
    }
    if (!fs::is_directory(directory)) {
        cerr << "Error: directory \"" << directory << "\" does not exist" << endl;
        return 1;
    }

    ::ThreadPool pool(std::thread::hardware_concurrency());
    pool.init();

    bool ok = true;
    fs::directory_iterator it(directory);
    for (auto entry : it) {
        fs::path p = it->path();
        if (!fs::is_regular_file(p) || !p.has_extension()) {
            continue;
        }
        if (p.extension() != ".mca") {
            continue;
        }
        auto region = Region::MakeRegion(p);
        if (!region) {
            continue;
        }
        auto beforeSize = fs::file_size(p);
        auto start = std::chrono::high_resolution_clock::now();
        cout << p.filename().string() << " " << (float)(beforeSize / 1024.0f) << " KiB -> ";
        cout.flush();
        vector<future<bool>> futures;
        for (int cx = region->minChunkX(); cx <= region->maxChunkX(); cx++) {
            for (int cz = region->minChunkZ(); cz <= region->maxChunkZ(); cz++) {
                futures.emplace_back(move(pool.submit([](shared_ptr<Region> region, int cx, int cz, fs::path temp) {
                    fs::path chunkFile = temp / Region::GetDefaultCompressedChunkNbtFileName(cx, cz);
                    return region->exportToZopfliCompressedNbt(cx, cz, chunkFile);
                },
                                                      region, cx, cz, *temp)));
            }
        }
        for (auto &f : futures) {
            if (!f.get()) {
                cerr << "Error: cannot export some chunks" << endl;
                ok = false;
                break;
            }
        }
        if (!ok) {
            break;
        }
        if (!Region::ConcatCompressedNbt(region->fX, region->fZ, *temp, p)) {
            cerr << "Error: failed concat'ing .nbt.z files" << endl;
            ok = false;
            break;
        }

        fs::remove_all(*temp);
        fs::create_directories(*temp);

        auto finish = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(finish - start).count();
        auto afterSize = fs::file_size(p);
        cout << (float)(afterSize / 1024.0f) << " KiB";
        if (afterSize < beforeSize) {
            float delta = (beforeSize - afterSize) * 100.0f / beforeSize;
            cout << " (-" << delta << "%";
        } else {
            float delta = (afterSize - beforeSize) * 100.0f / beforeSize;
            cout << " (+" << delta << "%";
        }
        cout << ", " << (float)elapsed << " sec)" << endl;
    }

    pool.shutdown();

    fs::remove_all(*temp);

    return ok ? 0 : 1;
}
