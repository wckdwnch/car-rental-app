#pragma once

#include <gtkmm.h>
#include "../Database/cusrepo.h"

class CustomersView : public Gtk::Box {
public:
    explicit CustomersView(CusRepo& cusRepo);
    void loadCustomers();

private:
    CusRepo& cusRepo_;

    Gtk::Label titleLabel_;
    Gtk::Entry* searchEntry_ = nullptr;

    Gtk::TreeView treeView_;
    Glib::RefPtr<Gtk::ListStore> listStore_;

    Gtk::Button btnAdd_;
    Gtk::Button btnEdit_;
    Gtk::Button btnRemove_;

    class Columns : public Gtk::TreeModel::ColumnRecord {
    public:
        Columns() {
            add(id);
            add(name);
            add(phone);
            add(license);
            add(email);
        }

        Gtk::TreeModelColumn<int> id;
        Gtk::TreeModelColumn<Glib::ustring> name;
        Gtk::TreeModelColumn<Glib::ustring> phone;
        Gtk::TreeModelColumn<Glib::ustring> license;
        Gtk::TreeModelColumn<Glib::ustring> email;
    };

    Columns columns_;

    void open_customer_form(bool isEdit, int customerId = 0);

    void on_add_clicked();
    void on_edit_clicked();
    void on_remove_clicked();
};