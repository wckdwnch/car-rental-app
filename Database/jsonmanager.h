#pragma once

#include <string>

#include "carrepo.h"
#include "cusrepo.h"
#include "rentalrepo.h"

class JsonManager {
public:
    JsonManager(CarRepo& carRepo, CusRepo& cusRepo, RentalRepo& rentalRepo);

    void exportToJson(const std::string& filename);
    void importFromJson(const std::string& filename);

private:
    CarRepo& carRepo_;
    CusRepo& cusRepo_;
    RentalRepo& rentalRepo_;
};