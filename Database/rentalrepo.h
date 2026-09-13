#pragma once

#include <vector>
#include "../Model/Rental.h"
#include "dbmanager.h"

class RentalRepo {
public:
    explicit RentalRepo(DbManager& dbManager);

    std::vector<Rental> getAllRentals();

    void assignCar(int carId, int customerId);
    void markReturned(int rentalId);
    bool isCarRented(int carId);
    void clearRentals();
    void addRentalWithId(const Rental& rental);

private:
    DbManager& dbManager_;
};