#include "MainWindow.h"

#include <iostream>
#include <exception>
#include <string>

namespace {
    constexpr const char* APP_NAME = "Latte Cruisers";
    constexpr const char* DATABASE_FILE = "car_rental.db";

    constexpr const char* CARS_PAGE = "cars";
    constexpr const char* CUSTOMERS_PAGE = "customers";
    constexpr const char* RENTALS_PAGE = "rentals";

    constexpr const char* DEFAULT_BACKUP_FILE = "latte_cruisers_backup.json";
}

MainWindow::MainWindow()
: dbManager_(DATABASE_FILE),
  carRepo_(dbManager_),
  cusRepo_(dbManager_),
  rentalRepo_(dbManager_),
  jsonManager_(carRepo_, cusRepo_, rentalRepo_),
  carsView_(carRepo_),
  customersView_(cusRepo_),
  rentalsView_(carRepo_, cusRepo_, rentalRepo_)
{
    set_title(APP_NAME);
    set_default_size(1100, 700);

    auto header = Gtk::make_managed<Gtk::HeaderBar>();
    header->set_show_title_buttons(true);

    auto titleBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    titleBox->set_spacing(0);
    titleBox->set_halign(Gtk::Align::CENTER);

    auto appTitle = Gtk::make_managed<Gtk::Label>(APP_NAME);
    appTitle->add_css_class("app-title");
    appTitle->set_halign(Gtk::Align::CENTER);

    auto appSubtitle = Gtk::make_managed<Gtk::Label>(
        "Rent a car from our cruiser collection and enjoy a free latte with every ride."
    );
    appSubtitle->add_css_class("app-subtitle");
    appSubtitle->set_halign(Gtk::Align::CENTER);

    titleBox->append(*appTitle);
    titleBox->append(*appSubtitle);

    header->set_title_widget(*titleBox);
    set_titlebar(*header);

    auto mainLayout = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    set_child(*mainLayout);

    auto coffeeSidebar = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    coffeeSidebar->set_size_request(185, -1);
    coffeeSidebar->add_css_class("sidebar-container");
    coffeeSidebar->set_margin_top(20);
    coffeeSidebar->set_margin_start(12);
    coffeeSidebar->set_margin_end(12);
    coffeeSidebar->set_margin_bottom(12);
    coffeeSidebar->set_spacing(14);

    auto latteFleetButton = Gtk::make_managed<Gtk::Button>("Latte Fleet");
    auto customersButton = Gtk::make_managed<Gtk::Button>("Customers");
    auto rentalsButton = Gtk::make_managed<Gtk::Button>("Rentals");

    latteFleetButton->add_css_class("sidebar-button");
    customersButton->add_css_class("sidebar-button");
    rentalsButton->add_css_class("sidebar-button");

    coffeeSidebar->append(*latteFleetButton);
    coffeeSidebar->append(*customersButton);
    coffeeSidebar->append(*rentalsButton);

    auto sidebarSpacer = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    sidebarSpacer->set_vexpand(true);
    coffeeSidebar->append(*sidebarSpacer);

    auto saveBackupButton = Gtk::make_managed<Gtk::Button>("Save Backup");
    auto loadBackupButton = Gtk::make_managed<Gtk::Button>("Load Backup");

    saveBackupButton->add_css_class("sidebar-action-button");
    loadBackupButton->add_css_class("sidebar-action-button");

    saveBackupButton->set_size_request(125, 38);
    loadBackupButton->set_size_request(125, 38);

    coffeeSidebar->append(*saveBackupButton);
    coffeeSidebar->append(*loadBackupButton);

    mainLayout->append(*coffeeSidebar);

    stack_.add(carsView_, CARS_PAGE);
    stack_.add(customersView_, CUSTOMERS_PAGE);
    stack_.add(rentalsView_, RENTALS_PAGE);

    mainLayout->append(stack_);
    stack_.set_visible_child(CARS_PAGE);

    latteFleetButton->signal_clicked().connect([this]() {
        stack_.set_visible_child(CARS_PAGE);
    });

    customersButton->signal_clicked().connect([this]() {
        stack_.set_visible_child(CUSTOMERS_PAGE);
    });

    rentalsButton->signal_clicked().connect([this]() {
        stack_.set_visible_child(RENTALS_PAGE);
    });

    saveBackupButton->signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::on_save_backup_clicked)
    );

    loadBackupButton->signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::on_load_backup_clicked)
    );
}

void MainWindow::on_save_backup_clicked()
{
    auto backupWindow = Gtk::make_managed<Gtk::Window>();
    backupWindow->set_title("");
    backupWindow->set_default_size(520, 260);
    backupWindow->set_modal(true);
    backupWindow->set_resizable(false);
    backupWindow->set_decorated(false);
    backupWindow->add_css_class("form-window");

    if (auto parent = dynamic_cast<Gtk::Window*>(get_root())) {
        backupWindow->set_transient_for(*parent);
    }

    auto backupCard = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    backupCard->add_css_class("popup-card");
    backupCard->set_spacing(14);

    auto backupTitle = Gtk::make_managed<Gtk::Label>("Save Backup");
    backupTitle->add_css_class("form-title");
    backupTitle->set_halign(Gtk::Align::CENTER);
    backupTitle->set_margin_top(18);

    auto backupHintLabel = Gtk::make_managed<Gtk::Label>(
        "Choose a filename for your Latte Cruisers backup:"
    );
    backupHintLabel->set_halign(Gtk::Align::START);
    backupHintLabel->set_margin_start(18);
    backupHintLabel->set_margin_end(18);

    auto backupPathEntry = Gtk::make_managed<Gtk::Entry>();
    backupPathEntry->set_text(DEFAULT_BACKUP_FILE);
    backupPathEntry->add_css_class("form-entry");
    backupPathEntry->set_size_request(440, 48);
    backupPathEntry->set_margin_start(18);
    backupPathEntry->set_margin_end(18);

    auto backupButtonRow = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    backupButtonRow->set_spacing(12);
    backupButtonRow->set_halign(Gtk::Align::CENTER);
    backupButtonRow->set_margin_bottom(18);

    auto cancelButton = Gtk::make_managed<Gtk::Button>("Cancel");
    auto saveButton = Gtk::make_managed<Gtk::Button>("Save");

    cancelButton->add_css_class("form-button");
    saveButton->add_css_class("form-button");

    cancelButton->set_size_request(120, 44);
    saveButton->set_size_request(120, 44);

    backupButtonRow->append(*cancelButton);
    backupButtonRow->append(*saveButton);

    backupCard->append(*backupTitle);
    backupCard->append(*backupHintLabel);
    backupCard->append(*backupPathEntry);
    backupCard->append(*backupButtonRow);

    backupWindow->set_child(*backupCard);

    cancelButton->signal_clicked().connect([backupWindow]() {
        backupWindow->close();
    });

    saveButton->signal_clicked().connect(
        [this, backupWindow, backupPathEntry]() {
            try {
                std::string filename = backupPathEntry->get_text();

                if (filename.empty()) {
                    std::cerr << "Save backup failed: filename is empty.\n";
                    return;
                }

                if (filename.size() < 5 ||
                    filename.substr(filename.size() - 5) != ".json") {
                    filename += ".json";
                }

                jsonManager_.exportToJson(filename);

                std::cout << "Backup saved to: " << filename << std::endl;
                backupWindow->close();
            }
            catch (const std::exception& e) {
                std::cerr << "Save backup failed: " << e.what() << std::endl;
            }
            catch (...) {
                std::cerr << "Save backup failed with unknown error.\n";
            }
        }
    );

    backupWindow->present();
}

void MainWindow::on_load_backup_clicked()
{
    auto loadBackupDialog = Gtk::FileChooserNative::create(
        "Load Backup",
        *this,
        Gtk::FileChooser::Action::OPEN,
        "Load",
        "Cancel"
    );

    auto jsonFilter = Gtk::FileFilter::create();
    jsonFilter->set_name("JSON backup files");
    jsonFilter->add_pattern("*.json");
    loadBackupDialog->add_filter(jsonFilter);

    loadBackupDialog->signal_response().connect(
        [this, loadBackupDialog](int response) {
            if (response != Gtk::ResponseType::ACCEPT) {
                return;
            }

            auto selectedFile = loadBackupDialog->get_file();

            if (!selectedFile) {
                return;
            }

            try {
                std::string filename = selectedFile->get_path();

                jsonManager_.importFromJson(filename);

                carsView_.loadCars();
                customersView_.loadCustomers();
                rentalsView_.loadRentals();

                std::cout << "Backup loaded from: " << filename << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Load backup failed: " << e.what() << std::endl;
            }
            catch (...) {
                std::cerr << "Load backup failed with unknown error.\n";
            }
        }
    );

    loadBackupDialog->show();
}