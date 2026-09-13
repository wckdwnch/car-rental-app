#pragma once

#include <vector>

#include "../Model/Car.h"
#include "dbmanager.h"

class CarRepo {
public:
    explicit CarRepo(DbManager& dbManager);

    std::vector<Car> getAllCars();
    void addCar(const Car& car);
    void updateCar(const Car& car);
    void removeCar(int id);
    void removeAllCars();
    void clearCars();
    void addCarWithId(const Car& car);

private:
    DbManager& dbManager_;
};