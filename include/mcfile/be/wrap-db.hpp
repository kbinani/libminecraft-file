#pragma once

namespace mcfile::be {

#if __has_include(<leveldb/db.h>)

class WrapDb : public DbInterface {
public:
    explicit WrapDb(leveldb::DB *unowned)
        : fDb(unowned) {}

    std::optional<std::string> get(std::string const &key) override {
        if (!fDb) {
            return std::nullopt;
        }
        leveldb::ReadOptions ro;
        ro.fill_cache = false;
        std::string v;
        if (!fDb->Get(ro, leveldb::Slice(key), &v).ok()) {
            return std::nullopt;
        }
        return v;
    }

public:
    leveldb::DB *const fDb;
};

#endif

} // namespace mcfile::be
