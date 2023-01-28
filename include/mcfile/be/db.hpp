#pragma once

#if __has_include(<leveldb/db.h>)

namespace mcfile::be {

class Db : public DbInterface {
public:
    static std::shared_ptr<Db> Open(std::string const &) = delete;
    static std::shared_ptr<Db> Open(std::wstring const &) = delete;

    static std::shared_ptr<Db> Open(std::filesystem::path const &dir) {
        using namespace leveldb;
        DB *db = nullptr;
        Options options;
        options.compression = kZlibRawCompression;
        if (!DB::Open(options, dir, &db).ok()) {
            return nullptr;
        }
        return std::shared_ptr<Db>(new Db(db));
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
    explicit Db(leveldb::DB *db)
        : fDb(db) {
    }

private:
    std::unique_ptr<leveldb::DB> fDb;
};

} // namespace mcfile::be

#endif
