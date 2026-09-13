//GTK/gtkmm is used for graphical user interface.
//The custome Latte Cruisers theme is loaded through Gtk::CssProvider

#include <gtkmm.h>
#include <iostream>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create("com.lattecruisers.rental");

    auto cssProvider = Gtk::CssProvider::create();

    try {
        cssProvider->load_from_path("style.css");
        std::cout << "LatteCars theme loaded successfully.\n";
    }
    catch (const Glib::Error& ex) {
        std::cerr << "Could not load LatteCars theme: " << ex.what() << std::endl;
    }

    Gtk::StyleContext::add_provider_for_display(
        Gdk::Display::get_default(),
        cssProvider,
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    return app->make_window_and_run<MainWindow>(argc, argv);
}