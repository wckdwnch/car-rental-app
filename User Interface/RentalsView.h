#pragma once

#include <gtkmm.h>
#include "../Database/carrepo.h"
#include "../Database/cusrepo.h"
#include "../Database/rentalrepo.h"

class RentalsView : public Gtk::Box {
public:
    RentalsView(CarRepo& carRepo, CusRepo& cusRepo, RentalRepo& rentalRepo);
    void loadRentals();

private:
    CarRepo& carRepo_;
    CusRepo& cusRepo_;
    RentalRepo& rentalRepo_;

    Gtk::Label titleLabel_;

    Gtk::Button btnAssign_;
    Gtk::Button btnUnassign_;

    Gtk::TreeView rentalsTree_;
    Glib::RefPtr<Gtk::ListStore> rentalsStore_;

    class RentalColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        RentalColumns() {
            add(id);
            add(carId);
            add(customerId);
            add(status);
        }

        Gtk::TreeModelColumn<int> id;
        Gtk::TreeModelColumn<int> carId;
        Gtk::TreeModelColumn<int> customerId;
        Gtk::TreeModelColumn<Glib::ustring> status;
    };

    class RentalCarColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        RentalCarColumns() {
            add(id);
            add(registration);
            add(brand);
            add(model);
        }

        Gtk::TreeModelColumn<int> id;
        Gtk::TreeModelColumn<Glib::ustring> registration;
        Gtk::TreeModelColumn<Glib::ustring> brand;
        Gtk::TreeModelColumn<Glib::ustring> model;
    };

    class RentalCustomerColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        RentalCustomerColumns() {
            add(id);
            add(name);
            add(email);
            add(phone);
        }

        Gtk::TreeModelColumn<int> id;
        Gtk::TreeModelColumn<Glib::ustring> name;
        Gtk::TreeModelColumn<Glib::ustring> email;
        Gtk::TreeModelColumn<Glib::ustring> phone;
    };

    RentalColumns rentalCols_;
    RentalCarColumns rentalCarCols_;
    RentalCustomerColumns rentalCustomerCols_;

    void on_assign_clicked();
    void on_unassign_clicked();
};