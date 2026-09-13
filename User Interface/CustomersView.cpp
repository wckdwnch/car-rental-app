#include "CustomersView.h"

#include <iostream>
#include <string>
#include <exception>

#include "../Model/Customer.h"
#include <algorithm>
#include <cctype>

static std::string toLower(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });

    return text;
}

static bool containsIgnoreCase(const std::string& text, const std::string& search)
{
    return toLower(text).find(toLower(search)) != std::string::npos;
}

CustomersView::CustomersView(CusRepo& cusRepo)
: Gtk::Box(Gtk::Orientation::VERTICAL),
  cusRepo_(cusRepo),
  btnAdd_("Add Customer"),
  btnEdit_("Edit Customer"),
  btnRemove_("Remove Customer")
{
    set_spacing(16);
    set_margin(24);

    set_hexpand(true);
    set_vexpand(true);
    set_halign(Gtk::Align::CENTER);
    set_valign(Gtk::Align::START);

    titleLabel_.set_text(" Cruiser Customers");
    titleLabel_.get_style_context()->add_class("title");
    titleLabel_.set_halign(Gtk::Align::START);
    append(titleLabel_);

    auto subtitle = Gtk::make_managed<Gtk::Label>(
        "Keep track of the customers booking rides from Latte Cruisers."
    );
    subtitle->add_css_class("page-subtitle");
    subtitle->set_halign(Gtk::Align::START);
    append(*subtitle);

    auto customerToolbar = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    customerToolbar->set_spacing(10);
    customerToolbar->set_margin_top(12);
    customerToolbar->set_margin_bottom(16);

    auto customerSearchEntry = Gtk::make_managed<Gtk::Entry>();
    customerSearchEntry->set_placeholder_text("Search name, phone, license, or email...");
    customerSearchEntry->set_hexpand(true);
    customerSearchEntry->add_css_class("search-entry");
    searchEntry_ = customerSearchEntry;

    btnAdd_.set_label("+ Add Customer");
    btnEdit_.set_label("Edit Customer");
    btnRemove_.set_label("Remove Customer");

    btnAdd_.add_css_class("primary-button");
    btnEdit_.add_css_class("secondary-button");
    btnRemove_.add_css_class("danger-button");

    btnAdd_.set_size_request(160, 46);
    btnEdit_.set_size_request(130, 46);
    btnRemove_.set_size_request(160, 46);

    customerToolbar->append(*customerSearchEntry);
    customerToolbar->append(btnAdd_);
    customerToolbar->append(btnEdit_);
    customerToolbar->append(btnRemove_);

    append(*customerToolbar);

    listStore_ = Gtk::ListStore::create(columns_);
    treeView_.set_model(listStore_);

    treeView_.append_column("Customer ID", columns_.id);
    treeView_.append_column("Full Name", columns_.name);
    treeView_.append_column("Phone", columns_.phone);
    treeView_.append_column("License", columns_.license);
    treeView_.append_column("Email", columns_.email);

    treeView_.set_size_request(760, 260);

    auto customerCard = Gtk::make_managed<Gtk::Frame>();
    customerCard->get_style_context()->add_class("card");
    customerCard->set_size_request(800, 300);
    customerCard->set_child(treeView_);

    append(*customerCard);

    btnAdd_.signal_clicked().connect(sigc::mem_fun(*this, &CustomersView::on_add_clicked));
    btnEdit_.signal_clicked().connect(sigc::mem_fun(*this, &CustomersView::on_edit_clicked));
    btnRemove_.signal_clicked().connect(sigc::mem_fun(*this, &CustomersView::on_remove_clicked));

    searchEntry_->signal_changed().connect([this]() {
        loadCustomers();
    });

    loadCustomers();
}

void CustomersView::on_add_clicked()
{
    open_customer_form(false);
}

void CustomersView::on_edit_clicked()
{
    auto selection = treeView_.get_selection();
    auto iter = selection->get_selected();

    if (!iter) {
        std::cout << "No customer selected\n";
        return;
    }

    Gtk::TreeModel::Row row = *iter;
    int selectedId = row[columns_.id];

    open_customer_form(true, selectedId);
}

void CustomersView::open_customer_form(bool isEdit, int customerId)
{
    Glib::ustring currentName;
    Glib::ustring currentPhone;
    Glib::ustring currentLicense;
    Glib::ustring currentEmail;

    if (isEdit) {
        auto selection = treeView_.get_selection();
        auto iter = selection->get_selected();

        if (!iter) {
            std::cout << "No customer selected\n";
            return;
        }

        Gtk::TreeModel::Row row = *iter;

        currentName = row[columns_.name];
        currentPhone = row[columns_.phone];
        currentLicense = row[columns_.license];
        currentEmail = row[columns_.email];
    }

    auto customerFormWindow = Gtk::make_managed<Gtk::Window>();
    customerFormWindow->set_title("");
    customerFormWindow->set_default_size(520, 520);
    customerFormWindow->set_modal(true);
    customerFormWindow->set_resizable(false);
    customerFormWindow->set_decorated(false);
    customerFormWindow->add_css_class("form-window");

    if (auto parent = dynamic_cast<Gtk::Window*>(get_root())) {
        customerFormWindow->set_transient_for(*parent);
    }

    auto customerFormCard = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    customerFormCard->add_css_class("popup-card");
    customerFormCard->set_spacing(14);

    auto formTopBar = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    formTopBar->set_margin_top(12);
    formTopBar->set_margin_start(12);
    formTopBar->set_margin_end(12);

    auto topBarSpacer = Gtk::make_managed<Gtk::Box>();
    topBarSpacer->set_hexpand(true);

    auto closeButton = Gtk::make_managed<Gtk::Button>("×");
    closeButton->add_css_class("popup-close-button");
    closeButton->set_size_request(44, 44);

    formTopBar->append(*topBarSpacer);
    formTopBar->append(*closeButton);

    auto formContent = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    formContent->set_spacing(12);
    formContent->set_margin_start(24);
    formContent->set_margin_end(24);
    formContent->set_margin_bottom(24);

    auto formTitle = Gtk::make_managed<Gtk::Label>(
        isEdit ? "Edit Customer Details" : "Add a Latte Cruisers Customer"
    );
    formTitle->add_css_class("form-title");
    formTitle->set_halign(Gtk::Align::CENTER);

    auto nameEntry = Gtk::make_managed<Gtk::Entry>();
    auto phoneEntry = Gtk::make_managed<Gtk::Entry>();
    auto emailEntry = Gtk::make_managed<Gtk::Entry>();

    nameEntry->set_placeholder_text("Customer Full Name");
    phoneEntry->set_placeholder_text("Phone Number");
    emailEntry->set_placeholder_text("Email Address");

    nameEntry->set_text(currentName);
    phoneEntry->set_text(currentPhone);
    emailEntry->set_text(currentEmail);

    nameEntry->add_css_class("form-entry");
    phoneEntry->add_css_class("form-entry");
    emailEntry->add_css_class("form-entry");

    nameEntry->set_size_request(420, 48);
    phoneEntry->set_size_request(420, 48);
    emailEntry->set_size_request(420, 48);

    auto licenseCheck = Gtk::make_managed<Gtk::CheckButton>("Has valid driver's license?");
    licenseCheck->add_css_class("license-toggle");
    licenseCheck->set_active(currentLicense == "Yes");

    auto submitButton = Gtk::make_managed<Gtk::Button>(
        isEdit ? "Save Customer" : "Add Customer"
    );
    auto cancelButton = Gtk::make_managed<Gtk::Button>("Cancel");

    submitButton->add_css_class("form-button");
    cancelButton->add_css_class("form-button");

    submitButton->set_size_request(140, 44);
    cancelButton->set_size_request(120, 44);

    auto formButtonRow = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    formButtonRow->set_spacing(12);
    formButtonRow->set_halign(Gtk::Align::CENTER);

    formButtonRow->append(*cancelButton);
    formButtonRow->append(*submitButton);

    formContent->append(*formTitle);
    formContent->append(*nameEntry);
    formContent->append(*phoneEntry);
    formContent->append(*licenseCheck);
    formContent->append(*emailEntry);
    formContent->append(*formButtonRow);

    customerFormCard->append(*formTopBar);
    customerFormCard->append(*formContent);

    customerFormWindow->set_child(*customerFormCard);

    closeButton->signal_clicked().connect([customerFormWindow]() {
        customerFormWindow->close();
    });

    cancelButton->signal_clicked().connect([customerFormWindow]() {
        customerFormWindow->close();
    });

    submitButton->signal_clicked().connect(
        [this, customerFormWindow, isEdit, customerId, nameEntry, phoneEntry, licenseCheck, emailEntry]() {
            try {
                Customer customer;
                customer.id = customerId;
                customer.name = nameEntry->get_text();
                customer.phone = phoneEntry->get_text();
                customer.license = licenseCheck->get_active() ? "Yes" : "No";
                customer.email = emailEntry->get_text();

                if (isEdit) {
                    cusRepo_.updateCustomer(customer);
                } else {
                    cusRepo_.addCustomer(customer);
                }

                loadCustomers();
                customerFormWindow->close();
            }
            catch (const std::exception& e) {
                std::cerr << "Could not save customer: " << e.what() << std::endl;
            }
        }
    );

    customerFormWindow->present();
}

void CustomersView::on_remove_clicked()
{
    auto selection = treeView_.get_selection();
    auto iter = selection->get_selected();

    if (!iter) {
        std::cout << "No customer selected\n";
        return;
    }

    Gtk::TreeModel::Row row = *iter;
    int selectedId = row[columns_.id];

    try {
        cusRepo_.removeCustomer(selectedId);
        loadCustomers();
    }
    catch (const std::exception& e) {
        std::cerr << "Could not remove customer: " << e.what() << std::endl;
    }
}

void CustomersView::loadCustomers()
{
    listStore_->clear();

    std::string searchText;

    if (searchEntry_) {
        searchText = searchEntry_->get_text();
    }

    auto customers = cusRepo_.getAllCustomers();

    for (const auto& customer : customers) {
        bool matches =
            searchText.empty() ||
            containsIgnoreCase(customer.name, searchText) ||
            containsIgnoreCase(customer.phone, searchText) ||
            containsIgnoreCase(customer.license, searchText) ||
            containsIgnoreCase(customer.email, searchText);

        if (!matches) {
            continue;
        }

        auto row = *(listStore_->append());

        row[columns_.id] = customer.id;
        row[columns_.name] = customer.name;
        row[columns_.phone] = customer.phone;
        row[columns_.license] = customer.license;
        row[columns_.email] = customer.email;
    }
}