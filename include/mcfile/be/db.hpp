#pragma once

#if __has_include(<leveldb/db.h>)

namespace mcfile::be {

class Db : public DbInterface {
public:
    Db(std::string const &) = delete;
    Db(std::wstring const &) = delete;
    explicit Db(std::filesystem::path const &dir) : fDb(nullptr), fValid(false) {
        using namespace leveldb;

        DB *db;
        Options options;
        options.compression = kZlibRawCompression;
        Status status = DB::Open(options, dir, &db);
        if (!status.ok()) {
            return;
        }
        fDb = db;
        fValid = true;
    }

    ~Db() {
        if (fValid) {
            delete fDb;
        }
    }

    std::optional<std::string> get(std::string const &key) override {
        assert(fValid);
        leveldb::ReadOptions o;
        std::string v;
        leveldb::Status st = fDb->Get(o, key, &v);
        if (st.ok()) {
            return v;
        } else {
            return std::nullopt;
        }
    }

private:
    leveldb::DB *fDb;
    bool fValid;
};

}

#endif
