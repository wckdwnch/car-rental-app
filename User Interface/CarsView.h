#pragma once

#include <gtkmm.h>
#include "../Database/carrepo.h"

class CarsView : public Gtk::Box {
public:
    explicit CarsView(CarRepo& carRepo);
    void loadCars();

private:
    CarRepo& carRepo_;

    Gtk::Label titleLabel_;
    Gtk::Entry* searchEntry_ = nullptr;

    Gtk::TreeView treeView_;
    Glib::RefPtr<Gtk::ListStore> listStore_;

    Gtk::Button addVehicleButton_;
    Gtk::Button editVehicleButton_;
    Gtk::Button removeVehicleButton_;

    class Columns : public Gtk::TreeModel::ColumnRecord {
    public:
        Columns() {
            add(id);
            add(brand);
            add(model);
            add(productionYear);
            add(registration);
            add(dailyRate);
        }

        Gtk::TreeModelColumn<int> id;
        Gtk::TreeModelColumn<Glib::ustring> brand;
        Gtk::TreeModelColumn<Glib::ustring> model;
        Gtk::TreeModelColumn<int> productionYear;
        Gtk::TreeModelColumn<Glib::ustring> registration;
        Gtk::TreeModelColumn<double> dailyRate;
    };

    Columns columns_;

    void open_car_form(bool isEdit, int carId = 0);

    void on_add_clicked();
    void on_edit_clicked();
    void on_remove_clicked();
};