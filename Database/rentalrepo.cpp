#include "rentalrepo.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <stdexcept>

RentalRepo::RentalRepo(DbManager& dbManager)
    : dbManager_(dbManager)
{
}

std::vector<Rental> RentalRepo::getAllRentals()
{
    std::vector<Rental> rentals;

    SQLite::Statement query(
        dbManager_.getDatabase(),
        "SELECT id, carId, customerId, status FROM Rentals"
    );

    while (query.executeStep()) {
        Rental rental;

        rental.id = query.getColumn(0).getInt();
        rental.carId = query.getColumn(1).getInt();
        rental.customerId = query.getColumn(2).getInt();
        rental.status = query.getColumn(3).getString();

        rentals.push_back(rental);
    }

    return rentals;
}

void RentalRepo::assignCar(int carId, int customerId)
{
    SQLite::Statement query(
        dbManager_.getDatabase(),
        "INSERT INTO Rentals (carId, customerId, status) VALUES (?, ?, 'Active')"
    );

    query.bind(1, carId);
    query.bind(2, customerId);

    query.exec();
}

void RentalRepo::markReturned(int rentalId)
{
    SQLite::Statement query(
        dbManager_.getDatabase(),
        "UPDATE Rentals SET status = 'Returned' WHERE id = ?"
    );

    query.bind(1, rentalId);
    query.exec();
}

bool RentalRepo::isCarRented(int carId)
{
    SQLite::Statement query(
        dbManager_.getDatabase(),
        "SELECT COUNT(*) FROM Rentals WHERE carId = ? AND status = 'Active'"
    );

    query.bind(1, carId);

    if (query.executeStep()) {
        return query.getColumn(0).getInt() > 0;
    }

    return false;
}

void RentalRepo::clearRentals()
{
    dbManager_.getDatabase().exec("DELETE FROM Rentals;");
    dbManager_.getDatabase().exec("DELETE FROM sqlite_sequence WHERE name='Rentals';");
}

void RentalRepo::addRentalWithId(const Rental& rental)
{
    SQLite::Statement query(
        dbManager_.getDatabase(),
        "INSERT INTO Rentals (id, carId, customerId, status) "
        "VALUES (?, ?, ?, ?)"
    );

    query.bind(1, rental.id);
    query.bind(2, rental.carId);
    query.bind(3, rental.customerId);
    query.bind(4, rental.status);

    query.exec();
}