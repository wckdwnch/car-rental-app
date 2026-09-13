#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <memory>

class DbManager {
public:
    explicit DbManager(const std::string& dbPath);

    SQLite::Database& getDatabase();

    void createTables();

private:
    std::unique_ptr<SQLite::Database> db_;
};