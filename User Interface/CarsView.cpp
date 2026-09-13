#include "CarsView.h"
#include <iostream>
#include <string>
#include <exception>
#include "../Model/Car.h"
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

CarsView::CarsView(CarRepo& carRepo)
: Gtk::Box(Gtk::Orientation::VERTICAL),
carRepo_(carRepo),
addVehicleButton_("Add Car"),
editVehicleButton_("Edit Latte Car"),
removeVehicleButton_("Remove Car")
{
    set_spacing(16);
    set_margin(24);

    set_hexpand(true);
    set_vexpand(true);
    set_halign(Gtk::Align::CENTER);
    set_valign(Gtk::Align::START);

    titleLabel_.set_text("Latte Cars");
    titleLabel_.get_style_context()->add_class("title");
    titleLabel_.set_halign(Gtk::Align::START);
    append(titleLabel_);

    auto subtitle = Gtk::make_managed<Gtk::Label>(
     "Add, edit, and remove cars from your Latte Fleet."
 );
    subtitle->add_css_class("page-subtitle");
    subtitle->set_halign(Gtk::Align::START);
    append(*subtitle);

    auto toolbar = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    toolbar->set_spacing(10);
    toolbar->set_margin_top(12);
    toolbar->set_margin_bottom(16);

    auto searchEntry = Gtk::make_managed<Gtk::Entry>();
    searchEntry->set_placeholder_text("Search plate number, brand, model, or year...");
    searchEntry->set_hexpand(true);
    searchEntry->add_css_class("search-entry");
    searchEntry_ = searchEntry;

    addVehicleButton_.set_label("+ Add Car");
    editVehicleButton_.set_label("Edit Details");
    removeVehicleButton_.set_label("Remove Car");

    addVehicleButton_.add_css_class("primary-button");
    editVehicleButton_.add_css_class("secondary-button");
    removeVehicleButton_.add_css_class("danger-button");

    addVehicleButton_.set_size_request(140, 46);
    editVehicleButton_.set_size_request(120, 46);
    removeVehicleButton_.set_size_request(150, 46);

    toolbar->append(*searchEntry);
    toolbar->append(addVehicleButton_);
    toolbar->append(editVehicleButton_);
    toolbar->append(removeVehicleButton_);

    append(*toolbar);

    listStore_ = Gtk::ListStore::create(columns_);
    treeView_.set_model(listStore_);

    treeView_.append_column("Car ID", columns_.id);
    treeView_.append_column("Brand", columns_.brand);
    treeView_.append_column("Model", columns_.model);
    treeView_.append_column("Year", columns_.productionYear);
    treeView_.append_column("Plate Number", columns_.registration);
    treeView_.append_column("Daily Rate", columns_.dailyRate);

    treeView_.set_size_request(760, 300);

    auto frame = Gtk::make_managed<Gtk::Frame>();
    frame->get_style_context()->add_class("card");
    frame->set_size_request(800, 340);
    frame->set_child(treeView_);

    append(*frame);

    addVehicleButton_.signal_clicked().connect(
        sigc::mem_fun(*this, &CarsView::on_add_clicked)
    );

    editVehicleButton_.signal_clicked().connect(
        sigc::mem_fun(*this, &CarsView::on_edit_clicked)
    );

    removeVehicleButton_.signal_clicked().connect(
        sigc::mem_fun(*this, &CarsView::on_remove_clicked)
    );

    searchEntry_->signal_changed().connect([this]() {
        loadCars();
    });

    loadCars();
}

void CarsView::on_add_clicked()
{
    open_car_form(false);
}

void CarsView::on_edit_clicked()
{
    auto selection = treeView_.get_selection();
    auto iter = selection->get_selected();

    if (!iter) {
        std::cout << "No vehicle selected\n";
        return;
    }

    Gtk::TreeModel::Row row = *iter;
    int selectedId = row[columns_.id];

    open_car_form(true, selectedId);
}

void CarsView::open_car_form(bool isEdit, int carId)
{
    Glib::ustring currentBrand;
    Glib::ustring currentModel;
    Glib::ustring currentRegistration;
    int currentYear = 2026;
    double currentDailyRate = 0.0;

    if (isEdit) {
        auto selection = treeView_.get_selection();
        auto iter = selection->get_selected();

        if (!iter) {
            std::cout << "No vehicle selected\n";
            return;
        }

        Gtk::TreeModel::Row row = *iter;

        currentBrand = row[columns_.brand];
        currentModel = row[columns_.model];
        currentYear = row[columns_.productionYear];
        currentRegistration = row[columns_.registration];
        currentDailyRate = row[columns_.dailyRate];
    }

    auto window = Gtk::make_managed<Gtk::Window>();
    window->set_title("");
    window->set_default_size(520, 560);
    window->set_modal(true);
    window->set_resizable(false);
    window->set_decorated(false);
    window->add_css_class("form-window");

    if (auto parent = dynamic_cast<Gtk::Window*>(get_root())) {
        window->set_transient_for(*parent);
    }

    auto outerBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    outerBox->add_css_class("popup-card");
    outerBox->set_spacing(14);

    auto topBar = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    topBar->set_margin_top(12);
    topBar->set_margin_start(12);
    topBar->set_margin_end(12);

    auto spacer = Gtk::make_managed<Gtk::Box>();
    spacer->set_hexpand(true);

    auto closeButton = Gtk::make_managed<Gtk::Button>("×");
    closeButton->add_css_class("popup-close-button");
    closeButton->set_size_request(44, 44);

    topBar->append(*spacer);
    topBar->append(*closeButton);

    auto contentBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    contentBox->set_spacing(12);
    contentBox->set_margin_start(24);
    contentBox->set_margin_end(24);
    contentBox->set_margin_bottom(24);

    auto title = Gtk::make_managed<Gtk::Label>(
        isEdit ? "Edit Car Details" : "Add New Car"
    );
    title->add_css_class("form-title");
    title->set_halign(Gtk::Align::CENTER);

    auto brandEntry = Gtk::make_managed<Gtk::Entry>();
    auto modelEntry = Gtk::make_managed<Gtk::Entry>();
    auto registrationEntry = Gtk::make_managed<Gtk::Entry>();

    brandEntry->set_placeholder_text("Brand");
    modelEntry->set_placeholder_text("Model");
    registrationEntry->set_placeholder_text("Plate Number");

    brandEntry->set_text(currentBrand);
    modelEntry->set_text(currentModel);
    registrationEntry->set_text(currentRegistration);

    brandEntry->add_css_class("form-entry");
    modelEntry->add_css_class("form-entry");
    registrationEntry->add_css_class("form-entry");

    brandEntry->set_size_request(420, 48);
    modelEntry->set_size_request(420, 48);
    registrationEntry->set_size_request(420, 48);

    auto yearAdjustment = Gtk::Adjustment::create(
        currentYear,
        1900,
        2100,
        1.0,
        10.0,
        0.0
    );

    auto yearSpin = Gtk::make_managed<Gtk::SpinButton>(yearAdjustment);
    yearSpin->set_digits(0);
    yearSpin->set_numeric(true);
    yearSpin->set_value(currentYear);
    yearSpin->add_css_class("form-entry");
    yearSpin->set_size_request(420, 48);

    auto dailyRateAdjustment = Gtk::Adjustment::create(
        currentDailyRate,
        0.0,
        100000.0,
        10.0,
        100.0,
        0.0
    );

    auto dailyRateSpin = Gtk::make_managed<Gtk::SpinButton>(dailyRateAdjustment);
    dailyRateSpin->set_digits(2);
    dailyRateSpin->set_numeric(true);
    dailyRateSpin->set_value(currentDailyRate);
    dailyRateSpin->add_css_class("form-entry");
    dailyRateSpin->set_size_request(420, 48);

    auto submitButton = Gtk::make_managed<Gtk::Button>(
        isEdit ? "Save Changes" : "Add Car"
    );
    auto cancelButton = Gtk::make_managed<Gtk::Button>("Cancel");

    submitButton->add_css_class("form-button");
    cancelButton->add_css_class("form-button");

    submitButton->set_size_request(140, 44);
    cancelButton->set_size_request(120, 44);

    auto buttonBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    buttonBox->set_spacing(12);
    buttonBox->set_halign(Gtk::Align::CENTER);

    buttonBox->append(*cancelButton);
    buttonBox->append(*submitButton);

    contentBox->append(*title);
    contentBox->append(*brandEntry);
    contentBox->append(*modelEntry);
    contentBox->append(*yearSpin);
    contentBox->append(*registrationEntry);
    contentBox->append(*dailyRateSpin);
    contentBox->append(*buttonBox);

    outerBox->append(*topBar);
    outerBox->append(*contentBox);

    window->set_child(*outerBox);

    closeButton->signal_clicked().connect([window]() {
        window->close();
    });

    cancelButton->signal_clicked().connect([window]() {
        window->close();
    });

    submitButton->signal_clicked().connect(
        [this, window, isEdit, carId, brandEntry, modelEntry, yearSpin, registrationEntry, dailyRateSpin]() {
            try {
                Car car;
                car.id = carId;
                car.brand = brandEntry->get_text();
                car.model = modelEntry->get_text();
                car.productionYear = yearSpin->get_value_as_int();
                car.registrationNumber = registrationEntry->get_text();
                car.dailyRate = dailyRateSpin->get_value();

                if (isEdit) {
                    carRepo_.updateCar(car);
                } else {
                    carRepo_.addCar(car);
                }

                loadCars();
                window->close();
            }
            catch (const std::exception& e) {
                std::cerr << "Could not save car: " << e.what() << std::endl;
            }
        }
    );

    window->present();
}

void CarsView::on_remove_clicked()
{
    auto selection = treeView_.get_selection();
    auto iter = selection->get_selected();

    if (!iter) {
        std::cout << "No car selected\n";
        return;
    }

    Gtk::TreeModel::Row row = *iter;
    int selectedId = row[columns_.id];

    try {
        carRepo_.removeCar(selectedId);
        loadCars();
    }
    catch (const std::exception& e) {
        std::cerr << "Could not remove car: " << e.what() << std::endl;
    }
}

void CarsView::loadCars()
{
    listStore_->clear();

    std::string searchText;

    if (searchEntry_) {
        searchText = searchEntry_->get_text();
    }

    auto cars = carRepo_.getAllCars();

    for (const auto& car : cars) {
        bool matches =
            searchText.empty() ||
            containsIgnoreCase(car.brand, searchText) ||
            containsIgnoreCase(car.model, searchText) ||
            containsIgnoreCase(car.registrationNumber, searchText) ||
            containsIgnoreCase(std::to_string(car.productionYear), searchText);

        if (!matches) {
            continue;
        }

        auto row = *(listStore_->append());

        row[columns_.id] = car.id;
        row[columns_.brand] = car.brand;
        row[columns_.model] = car.model;
        row[columns_.productionYear] = car.productionYear;
        row[columns_.registration] = car.registrationNumber;
        row[columns_.dailyRate] = car.dailyRate;
    }
}