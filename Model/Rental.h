#pragma once
#include <string>

struct Rental {
    int id = 0;
    int carId = 0;
    int customerId = 0;
    std::string status = "Active";
};