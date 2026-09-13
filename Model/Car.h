#pragma once

#include <string>

struct Car {
    int id = 0;
    std::string brand;
    std::string model;
    int productionYear = 0;
    std::string registrationNumber;
    double dailyRate = 0.0;
};