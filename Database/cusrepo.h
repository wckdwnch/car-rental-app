#pragma once

#include <vector>
#include "../Model/Customer.h"
#include "dbmanager.h"

class CusRepo {
public:
    explicit CusRepo(DbManager& dbManager);

    std::vector<Customer> getAllCustomers();
    void addCustomer(const Customer& customer);
    void updateCustomer(const Customer& customer);
    void removeCustomer(int id);
    void clearCustomers();
    void addCustomerWithId(const Customer& customer);

private:
    DbManager& dbManager_;
};