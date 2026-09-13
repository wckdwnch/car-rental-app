#include "carrepo.h"
#include <SQLiteCpp/SQLiteCpp.h>

CarRepo::CarRepo(DbManager& dbManager)
    : dbManager_(dbManager)
{
}

std::vector<Car> CarRepo::getAllCars()
{
    std::vector<Car> cars;

    SQLite::Statement query(
        dbManager_.getDatabase(),
        "SELECT id, brand, model, productionYear, registrationNumber, dailyRate FROM Cars"
    );

    while (query.executeStep()) {
        Car car;

        car.id = query.getColumn(0).getInt();
        car.brand = query.getColumn(1).getString();
        car.model = query.getColumn(2).getString();
        car.productionYear = query.getColumn(3).getInt();
        car.registrationNumber = query.getColumn(4).getString();
        car.dailyRate = query.getColumn(5).getDouble();

        cars.push_back(car);
    }

    return cars;
}

void CarRepo::addCar(const Car& car)
{
    SQLite::Statement query(
        dbManager_.getDatabase(),
        "INSERT INTO Cars (brand, model, productionYear, registrationNumber, dailyRate)" "VALUES (?, ?, ?, ?, ?)"
    );

    query.bind(1, car.brand);
    query.bind(2, car.model);
    query.bind(3, car.productionYear);
    query.bind(4, car.registrationNumber);
    query.bind(5, car.dailyRate);

    query.exec();
}

void CarRepo::updateCar(const Car& car)
{
    SQLite::Statement query(
        dbManager_.getDatabase(),
        "UPDATE Cars "
        "SET brand = ?, "
        "model = ?, "
        "productionYear = ?, "
        "registrationNumber = ?, "
        "dailyRate = ? "
        "WHERE id = ?"
    );

    query.bind(1, car.brand);
    query.bind(2, car.model);
    query.bind(3, car.productionYear);
    query.bind(4, car.registrationNumber);
    query.bind(5, car.dailyRate);
    query.bind(6, car.id);

    query.exec();
}

void CarRepo::removeCar(int id)
{
    SQLite::Statement query(
        dbManager_.getDatabase(),
        "DELETE FROM Cars WHERE id = ?"
    );

    query.bind(1, id);
    query.exec();
}

void CarRepo::removeAllCars()
    {
        dbManager_.getDatabase().exec("DELETE FROM Cars;");
        dbManager_.getDatabase().exec("DELETE FROM sqlite_sequence WHERE name='Cars';");
    }

void CarRepo::clearCars()
{
    dbManager_.getDatabase().exec("DELETE FROM Cars;");
    dbManager_.getDatabase().exec("DELETE FROM sqlite_sequence WHERE name='Cars';");
}

void CarRepo::addCarWithId(const Car& car)
{
    SQLite::Statement query(
        dbManager_.getDatabase(),
        "INSERT INTO Cars (id, brand, model, productionYear, registrationNumber, dailyRate) "
        "VALUES (?, ?, ?, ?, ?, ?)"
    );

    query.bind(1, car.id);
    query.bind(2, car.brand);
    query.bind(3, car.model);
    query.bind(4, car.productionYear);
    query.bind(5, car.registrationNumber);
    query.bind(6, car.dailyRate);

    query.exec();
}