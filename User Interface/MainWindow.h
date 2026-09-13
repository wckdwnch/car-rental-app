#pragma once

#include <gtkmm.h>
#include "CarsView.h"
#include "CustomersView.h"
#include "RentalsView.h"
#include "../Database/dbmanager.h"
#include "../Database/carrepo.h"
#include "../Database/cusrepo.h"
#include "../Database/rentalrepo.h"
#include "../Database/jsonmanager.h"

class MainWindow : public Gtk::Window {
public:
    MainWindow();

private:
    DbManager dbManager_;

    CarRepo carRepo_;
    CusRepo cusRepo_;
    RentalRepo rentalRepo_;

    JsonManager jsonManager_;

    Gtk::Stack stack_;

    CarsView carsView_;
    CustomersView customersView_;
    RentalsView rentalsView_;

    void on_save_backup_clicked();
    void on_load_backup_clicked();
};