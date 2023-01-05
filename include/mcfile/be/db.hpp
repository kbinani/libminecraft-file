#pragma once

#if __has_include(<leveldb/db.h>)

namespace mcfile::be {

class Db : public DbInterface {
public:
    Db(std::string const &) = delete;
    Db(std::wstring const &) = delete;
    explicit Db(std::filesystem::path const &dir)
        : fDb(nullptr) {
        using namespace leveldb;
        DB *db = nullptr;
        Options options;
        options.compression = kZlibRawCompression;
        if (!DB::Open(options, dir, &db).ok()) {
            return;
        }
        fDb.reset(db);
    }

    ~Db() {}

    std::optional<std::string> get(std::string const &key) override {
        assert(fDb);
        std::string v;
        leveldb::ReadOptions ro;
        ro.fill_cache = false;
        leveldb::Status st = fDb->Get(ro, key, &v);
        if (st.ok()) {
            return v;
        } else {
            return std::nullopt;
        }
    }

private:
    std::unique_ptr<leveldb::DB> fDb;
};

} // namespace mcfile::be

#endif
