#include "cusrepo.h"
#include <SQLiteCpp/SQLiteCpp.h>

CusRepo::CusRepo(DbManager& dbManager)
    : dbManager_(dbManager)
{
}

std::vector<Customer> CusRepo::getAllCustomers()
{
    std::vector<Customer> customers;

    SQLite::Statement query(
        dbManager_.getDatabase(),
        "SELECT id, name, phone, license, email FROM Customers"
    );

    while (query.executeStep()) {
        Customer customer;

        customer.id = query.getColumn(0).getInt();
        customer.name = query.getColumn(1).getString();
        customer.phone = query.getColumn(2).getString();
        customer.license = query.getColumn(3).getString();
        customer.email = query.getColumn(4).getString();

        customers.push_back(customer);
    }

    return customers;
}

void CusRepo::addCustomer(const Customer& customer)
{
    SQLite::Statement query(
        dbManager_.getDatabase(),
        "INSERT INTO Customers (name, phone, license, email) VALUES (?, ?, ?, ?)"
    );

    query.bind(1, customer.name);
    query.bind(2, customer.phone);
    query.bind(3, customer.license);
    query.bind(4, customer.email);

    query.exec();
}

void CusRepo::updateCustomer(const Customer& customer)
{
    SQLite::Statement query(
        dbManager_.getDatabase(),
        "UPDATE Customers SET name = ?, phone = ?, license = ?, email = ? WHERE id = ?"
    );

    query.bind(1, customer.name);
    query.bind(2, customer.phone);
    query.bind(3, customer.license);
    query.bind(4, customer.email);
    query.bind(5, customer.id);

    query.exec();
}

void CusRepo::removeCustomer(int id)
{
    SQLite::Statement query(
        dbManager_.getDatabase(),
        "DELETE FROM Customers WHERE id = ?"
    );

    query.bind(1, id);
    query.exec();
}

void CusRepo::clearCustomers()
{
    dbManager_.getDatabase().exec("DELETE FROM Customers;");
    dbManager_.getDatabase().exec("DELETE FROM sqlite_sequence WHERE name='Customers';");
}

void CusRepo::addCustomerWithId(const Customer& customer)
{
    SQLite::Statement query(
        dbManager_.getDatabase(),
        "INSERT INTO Customers (id, name, phone, license, email) "
        "VALUES (?, ?, ?, ?, ?)"
    );

    query.bind(1, customer.id);
    query.bind(2, customer.name);
    query.bind(3, customer.phone);
    query.bind(4, customer.license);
    query.bind(5, customer.email);

    query.exec();
}