#include "dbmanager.h"

#include <cairomm/context.h>
#include <gtkmm/accessible.h>

DbManager::DbManager(const std::string& dbPath)
{
    db_ = std::make_unique<SQLite::Database>(
        dbPath,
        SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE
    );

    createTables();
}

SQLite::Database& DbManager::getDatabase()
{
    return *db_;
}

void DbManager::createTables()
{
    db_->exec(R"(
    CREATE TABLE IF NOT EXISTS Cars (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        brand TEXT NOT NULL,
        model TEXT NOT NULL,
        productionYear INTEGER NOT NULL,
        registrationNumber TEXT NOT NULL UNIQUE,
        dailyRate REAL DEFAULT 0
    );
)");


    db_->exec(R"(
        CREATE TABLE IF NOT EXISTS Customers (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            phone TEXT,
            license TEXT,
            email TEXT
        );
    )");

    db_->exec(R"(
        CREATE TABLE IF NOT EXISTS Rentals (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            carId INTEGER NOT NULL,
            customerId INTEGER NOT NULL,
            status TEXT NOT NULL DEFAULT 'Active',
            FOREIGN KEY (carId) REFERENCES Cars(id),
            FOREIGN KEY (customerId) REFERENCES Customers(id)
        );
    )");
}