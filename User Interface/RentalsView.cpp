#include "RentalsView.h"

#include <iostream>
#include <exception>
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

RentalsView::RentalsView(CarRepo& carRepo, CusRepo& cusRepo, RentalRepo& rentalRepo)
: Gtk::Box(Gtk::Orientation::VERTICAL),
  carRepo_(carRepo),
  cusRepo_(cusRepo),
  rentalRepo_(rentalRepo),
  btnAssign_("Assign Car"),
  btnUnassign_("Unassign Car")
{
    set_spacing(16);
    set_margin(24);

    set_hexpand(true);
    set_vexpand(true);
    set_halign(Gtk::Align::CENTER);
    set_valign(Gtk::Align::START);

    titleLabel_.set_text("Cruiser Rentals");
    titleLabel_.get_style_context()->add_class("title");
    titleLabel_.set_halign(Gtk::Align::START);
    append(titleLabel_);

    auto subtitle = Gtk::make_managed<Gtk::Label>("Assign cars to customers and track every Latte Cruisers ride");
    subtitle->add_css_class("page-subtitle");
    subtitle->set_halign(Gtk::Align::START);
    append(*subtitle);

    auto toolbar = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    toolbar->set_spacing(10);
    toolbar->set_margin_top(12);
    toolbar->set_margin_bottom(16);

    btnAssign_.set_label("Assign Car");
    btnUnassign_.set_label("Mark Returned");

    btnAssign_.add_css_class("primary-button");
    btnUnassign_.add_css_class("danger-button");

    btnAssign_.set_size_request(150, 46);
    btnUnassign_.set_size_request(160, 46);

    toolbar->append(btnAssign_);
    toolbar->append(btnUnassign_);

    append(*toolbar);

    rentalsStore_ = Gtk::ListStore::create(rentalCols_);
    rentalsTree_.set_model(rentalsStore_);

    rentalsTree_.append_column("Rental ID", rentalCols_.id);
    rentalsTree_.append_column("Car ID", rentalCols_.carId);
    rentalsTree_.append_column("Customer ID", rentalCols_.customerId);
    rentalsTree_.append_column("Ride Status", rentalCols_.status);

    rentalsTree_.set_size_request(760, 300);

    auto frame = Gtk::make_managed<Gtk::Frame>();
    frame->get_style_context()->add_class("card");
    frame->set_size_request(800, 340);
    frame->set_child(rentalsTree_);

    append(*frame);

    btnAssign_.signal_clicked().connect(sigc::mem_fun(*this, &RentalsView::on_assign_clicked));
    btnUnassign_.signal_clicked().connect(sigc::mem_fun(*this, &RentalsView::on_unassign_clicked));

    loadRentals();
}

void RentalsView::on_assign_clicked()
{
    auto window = Gtk::make_managed<Gtk::Window>();
    window->set_title("Assign Car a Latte Cruiser");
    window->set_default_size(760, 620);
    window->set_modal(true);
    window->set_resizable(false);

    if (auto parent = dynamic_cast<Gtk::Window*>(get_root())) {
        window->set_transient_for(*parent);
    }

    auto mainBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    mainBox->set_spacing(14);
    mainBox->set_margin(16);

    auto title = Gtk::make_managed<Gtk::Label>("Assign a Car to a Customer");
    title->add_css_class("form-title");
    title->set_halign(Gtk::Align::START);
    mainBox->append(*title);

    auto topBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    topBox->set_spacing(14);

    // =========================
    // Available cars section
    // =========================
    auto carsBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    carsBox->set_spacing(8);

    auto carsLabel = Gtk::make_managed<Gtk::Label>("Available Latte Cruisers");
    carsLabel->set_halign(Gtk::Align::START);
    carsLabel->add_css_class("section-title");

    auto carSearch = Gtk::make_managed<Gtk::Entry>();
    carSearch->set_placeholder_text("Search registration, brand, or model...");
    carSearch->add_css_class("search-entry");

    auto carsStore = Gtk::ListStore::create(rentalCarCols_);
    auto carsTree = Gtk::make_managed<Gtk::TreeView>();
    carsTree->set_model(carsStore);

    carsTree->append_column("Car ID", rentalCarCols_.id);
    carsTree->append_column("Plate Number", rentalCarCols_.registration);
    carsTree->append_column("Brand", rentalCarCols_.brand);
    carsTree->append_column("Model", rentalCarCols_.model);
    carsTree->set_size_request(350, 280);

    auto cars = carRepo_.getAllCars();

    auto refreshCars = [this, carsStore, cars](const std::string& searchText) {
        carsStore->clear();

        for (const auto& car : cars) {
            if (rentalRepo_.isCarRented(car.id)) {
                continue;
            }

            bool matches =
                searchText.empty() ||
                containsIgnoreCase(car.registrationNumber, searchText) ||
                containsIgnoreCase(car.brand, searchText) ||
                containsIgnoreCase(car.model, searchText);

            if (!matches) {
                continue;
            }

            auto row = *(carsStore->append());
            row[rentalCarCols_.id] = car.id;
            row[rentalCarCols_.registration] = car.registrationNumber;
            row[rentalCarCols_.brand] = car.brand;
            row[rentalCarCols_.model] = car.model;
        }
    };

    refreshCars("");

    carSearch->signal_changed().connect([carSearch, refreshCars]() {
        refreshCars(std::string(carSearch->get_text()));
    });

    carsBox->append(*carsLabel);
    carsBox->append(*carSearch);
    carsBox->append(*carsTree);

    // =========================
    // Customers section
    // =========================
    auto customersBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    customersBox->set_spacing(8);

    auto customersLabel = Gtk::make_managed<Gtk::Label>("Customer Details");
    customersLabel->set_halign(Gtk::Align::START);
    customersLabel->add_css_class("section-title");

    auto customerSearch = Gtk::make_managed<Gtk::Entry>();
    customerSearch->set_placeholder_text("Search name, email, or phone...");
    customerSearch->add_css_class("search-entry");

    auto customersStore = Gtk::ListStore::create(rentalCustomerCols_);
    auto customersTree = Gtk::make_managed<Gtk::TreeView>();
    customersTree->set_model(customersStore);

    customersTree->append_column("Customer ID", rentalCustomerCols_.id);
    customersTree->append_column("Name", rentalCustomerCols_.name);
    customersTree->append_column("Email", rentalCustomerCols_.email);
    customersTree->append_column("Phone", rentalCustomerCols_.phone);
    customersTree->set_size_request(350, 280);

    auto customers = cusRepo_.getAllCustomers();

    auto refreshCustomers = [this, customersStore, customers](const std::string& searchText) {
        customersStore->clear();

        for (const auto& customer : customers) {
            bool matches =
                searchText.empty() ||
                containsIgnoreCase(customer.name, searchText) ||
                containsIgnoreCase(customer.email, searchText) ||
                containsIgnoreCase(customer.phone, searchText);

            if (!matches) {
                continue;
            }

            auto row = *(customersStore->append());
            row[rentalCustomerCols_.id] = customer.id;
            row[rentalCustomerCols_.name] = customer.name;
            row[rentalCustomerCols_.email] = customer.email;
            row[rentalCustomerCols_.phone] = customer.phone;
        }
    };

    refreshCustomers("");

    customerSearch->signal_changed().connect([customerSearch, refreshCustomers]() {
        refreshCustomers(std::string(customerSearch->get_text()));
    });

    customersBox->append(*customersLabel);
    customersBox->append(*customerSearch);
    customersBox->append(*customersTree);

    topBox->append(*carsBox);
    topBox->append(*customersBox);

    mainBox->append(*topBox);

    // =========================
    // Pickup date
    // =========================
    auto dateBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    dateBox->set_spacing(14);
    dateBox->set_margin_top(8);

    auto dateLabel = Gtk::make_managed<Gtk::Label>("Pickup date for this ride:");
    dateLabel->set_halign(Gtk::Align::START);

    auto calendar = Gtk::make_managed<Gtk::Calendar>();
    calendar->set_size_request(260, 190);

    dateBox->append(*dateLabel);
    dateBox->append(*calendar);

    mainBox->append(*dateBox);

    // =========================
    // Buttons
    // =========================
    auto buttonBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    buttonBox->set_spacing(10);
    buttonBox->set_halign(Gtk::Align::END);

    auto cancelButton = Gtk::make_managed<Gtk::Button>("Cancel");
    auto assignButton = Gtk::make_managed<Gtk::Button>("Assign Car");

    cancelButton->add_css_class("secondary-button");
    assignButton->add_css_class("primary-button");

    buttonBox->append(*cancelButton);
    buttonBox->append(*assignButton);

    mainBox->append(*buttonBox);

    window->set_child(*mainBox);

    cancelButton->signal_clicked().connect([window]() {
        window->close();
    });

    assignButton->signal_clicked().connect(
        [this, window, carsTree, customersTree]() {
            auto carSelection = carsTree->get_selection();
            auto customerSelection = customersTree->get_selection();

            auto carIter = carSelection->get_selected();
            auto customerIter = customerSelection->get_selected();

            if (!carIter || !customerIter) {
                std::cout << "Select both a car and a customer\n";
                return;
            }

            Gtk::TreeModel::Row carRow = *carIter;
            Gtk::TreeModel::Row customerRow = *customerIter;

            int carId = carRow[rentalCarCols_.id];
            int customerId = customerRow[rentalCustomerCols_.id];

            try {
                rentalRepo_.assignCar(carId, customerId);
                loadRentals();
                window->close();
            }
            catch (const std::exception& e) {
                std::cerr << "Could not assign car to customer: " << e.what() << std::endl;
            }
        }
    );

    window->present();
}

void RentalsView::on_unassign_clicked()
{
    auto selection = rentalsTree_.get_selection();
    auto iter = selection->get_selected();

    if (!iter) {
        std::cout << "No rental selected\n";
        return;
    }

    Gtk::TreeModel::Row row = *iter;
    int rentalId = row[rentalCols_.id];
    Glib::ustring currentStatus = row[rentalCols_.status];

    if (currentStatus == "Returned") {
        std::cout << "This Latte Cruisers ride is already returned\n";
        return;
    }

    try {
        rentalRepo_.markReturned(rentalId);
        loadRentals();
    }
    catch (const std::exception& e) {
        std::cerr << "Could not mark rental as returned: " << e.what() << std::endl;
    }
}

void RentalsView::loadRentals()
{
    rentalsStore_->clear();

    auto rentals = rentalRepo_.getAllRentals();

    for (const auto& rental : rentals) {
        auto row = *(rentalsStore_->append());

        row[rentalCols_.id] = rental.id;
        row[rentalCols_.carId] = rental.carId;
        row[rentalCols_.customerId] = rental.customerId;
        row[rentalCols_.status] = rental.status;
    }
}