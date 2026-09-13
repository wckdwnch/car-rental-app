#include "jsonmanager.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include "../Model/Car.h"
#include "../Model/Customer.h"
#include "../Model/Rental.h"

JsonManager::JsonManager(CarRepo& carRepo, CusRepo& cusRepo, RentalRepo& rentalRepo)
    : carRepo_(carRepo),
      cusRepo_(cusRepo),
      rentalRepo_(rentalRepo)
{
}

void JsonManager::exportToJson(const std::string& filename)
{
    rapidjson::Document doc;
    doc.SetObject();

    auto& allocator = doc.GetAllocator();

    rapidjson::Value carsArray(rapidjson::kArrayType);

    for (const auto& car : carRepo_.getAllCars()) {
        rapidjson::Value carObj(rapidjson::kObjectType);

        carObj.AddMember("id", car.id, allocator);

        rapidjson::Value brand;
        brand.SetString(car.brand.c_str(), static_cast<rapidjson::SizeType>(car.brand.length()), allocator);
        carObj.AddMember("brand", brand, allocator);

        rapidjson::Value model;
        model.SetString(car.model.c_str(), static_cast<rapidjson::SizeType>(car.model.length()), allocator);
        carObj.AddMember("model", model, allocator);

        carObj.AddMember("productionYear", car.productionYear, allocator);

        rapidjson::Value registration;
        registration.SetString(
            car.registrationNumber.c_str(),
            static_cast<rapidjson::SizeType>(car.registrationNumber.length()),
            allocator
        );
        carObj.AddMember("registrationNumber", registration, allocator);

        carObj.AddMember("dailyRate", car.dailyRate, allocator);

        carsArray.PushBack(carObj, allocator);
    }

    doc.AddMember("cars", carsArray, allocator);

    rapidjson::Value customersArray(rapidjson::kArrayType);

    for (const auto& customer : cusRepo_.getAllCustomers()) {
        rapidjson::Value customerObj(rapidjson::kObjectType);

        customerObj.AddMember("id", customer.id, allocator);

        rapidjson::Value name;
        name.SetString(customer.name.c_str(), static_cast<rapidjson::SizeType>(customer.name.length()), allocator);
        customerObj.AddMember("name", name, allocator);

        rapidjson::Value phone;
        phone.SetString(customer.phone.c_str(), static_cast<rapidjson::SizeType>(customer.phone.length()), allocator);
        customerObj.AddMember("phone", phone, allocator);

        rapidjson::Value license;
        license.SetString(customer.license.c_str(), static_cast<rapidjson::SizeType>(customer.license.length()), allocator);
        customerObj.AddMember("license", license, allocator);

        rapidjson::Value email;
        email.SetString(customer.email.c_str(), static_cast<rapidjson::SizeType>(customer.email.length()), allocator);
        customerObj.AddMember("email", email, allocator);

        customersArray.PushBack(customerObj, allocator);
    }

    doc.AddMember("customers", customersArray, allocator);

    rapidjson::Value rentalsArray(rapidjson::kArrayType);

    for (const auto& rental : rentalRepo_.getAllRentals()) {
        rapidjson::Value rentalObj(rapidjson::kObjectType);

        rentalObj.AddMember("id", rental.id, allocator);
        rentalObj.AddMember("carId", rental.carId, allocator);
        rentalObj.AddMember("customerId", rental.customerId, allocator);

        rapidjson::Value status;
        status.SetString(rental.status.c_str(), static_cast<rapidjson::SizeType>(rental.status.length()), allocator);
        rentalObj.AddMember("status", status, allocator);

        rentalsArray.PushBack(rentalObj, allocator);
    }

    doc.AddMember("rentals", rentalsArray, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    std::ofstream out(filename);

    if (!out.is_open()) {
        throw std::runtime_error("Could not open file for export.");
    }

    out << buffer.GetString();
}

void JsonManager::importFromJson(const std::string& filename)
{
    std::ifstream in(filename);

    if (!in.is_open()) {
        throw std::runtime_error("Could not open file for import.");
    }

    std::stringstream buffer;
    buffer << in.rdbuf();

    rapidjson::Document doc;
    doc.Parse(buffer.str().c_str());

    if (doc.HasParseError() || !doc.IsObject()) {
        throw std::runtime_error("Invalid JSON file.");
    }

    rentalRepo_.clearRentals();
    carRepo_.clearCars();
    cusRepo_.clearCustomers();

    if (doc.HasMember("cars") && doc["cars"].IsArray()) {
        for (const auto& item : doc["cars"].GetArray()) {
            Car car;

            if (item.HasMember("id") && item["id"].IsInt())
                car.id = item["id"].GetInt();

            if (item.HasMember("brand") && item["brand"].IsString())
                car.brand = item["brand"].GetString();

            if (item.HasMember("model") && item["model"].IsString())
                car.model = item["model"].GetString();

            if (item.HasMember("productionYear") && item["productionYear"].IsInt())
                car.productionYear = item["productionYear"].GetInt();

            if (item.HasMember("registrationNumber") && item["registrationNumber"].IsString())
                car.registrationNumber = item["registrationNumber"].GetString();

            if (item.HasMember("dailyRate") && item["dailyRate"].IsNumber())
                car.dailyRate = item["dailyRate"].GetDouble();

            carRepo_.addCarWithId(car);
        }
    }

    if (doc.HasMember("customers") && doc["customers"].IsArray()) {
        for (const auto& item : doc["customers"].GetArray()) {
            Customer customer;

            if (item.HasMember("id") && item["id"].IsInt())
                customer.id = item["id"].GetInt();

            if (item.HasMember("name") && item["name"].IsString())
                customer.name = item["name"].GetString();

            if (item.HasMember("phone") && item["phone"].IsString())
                customer.phone = item["phone"].GetString();

            if (item.HasMember("license") && item["license"].IsString())
                customer.license = item["license"].GetString();

            if (item.HasMember("email") && item["email"].IsString())
                customer.email = item["email"].GetString();

            cusRepo_.addCustomerWithId(customer);
        }
    }

    if (doc.HasMember("rentals") && doc["rentals"].IsArray()) {
        for (const auto& item : doc["rentals"].GetArray()) {
            Rental rental;

            if (item.HasMember("id") && item["id"].IsInt())
                rental.id = item["id"].GetInt();

            if (item.HasMember("carId") && item["carId"].IsInt())
                rental.carId = item["carId"].GetInt();

            if (item.HasMember("customerId") && item["customerId"].IsInt())
                rental.customerId = item["customerId"].GetInt();

            if (item.HasMember("status") && item["status"].IsString())
                rental.status = item["status"].GetString();
            else
                rental.status = "Active";

            rentalRepo_.addRentalWithId(rental);
        }
    }
}