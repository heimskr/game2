#include <iostream>

#include "App.h"
#include "UI.h"
#include "ui/InventoryDialog.h"
#include "ui/RegionTab.h"
#include "ui/TravelTab.h"

std::unique_ptr<App> app;

App::App(Glib::RefPtr<Gtk::Application> gtk_app): gtkApp(gtk_app) {
	updateDialogDispatcher.connect(sigc::mem_fun(*this, &App::updateDialog));

	mainWindow = std::make_unique<Gtk::ApplicationWindow>();
	builder = Gtk::Builder::create();
	builder->add_from_file("main.ui");

	saveButton = builder->get_widget<Gtk::Button>("save_button");
	saveButton->set_visible(false);

	cssProvider = Gtk::CssProvider::create();
	cssProvider->load_from_path("style.css");
	Gtk::StyleContext::add_provider_for_display(Gdk::Display::get_default(), cssProvider,
		GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	
	header = builder->get_widget<Gtk::HeaderBar>("headerbar");
	mainWindow->set_titlebar(*header);

	notebook = std::make_unique<Gtk::Notebook>();
	mainWindow->set_child(*notebook);
	mainWindow->set_default_size(1000, 600);
	notebook->hide();

	mainWindow->add_action("new", Gio::ActionMap::ActivateSlot([this] {
		notebook->show();
		auto lock = lockGame();
		app->game = Game::loadDefaults();
		saveButton->set_visible(true);
		onTravel();
		inventoryTab->reset();
	}));

	mainWindow->add_action("open", Gio::ActionMap::ActivateSlot([this] {
		notebook->show();
		auto lock = lockGame();
		app->game = Game::load();
		saveButton->set_visible(true);
		onTravel();
		inventoryTab->reset();
	}));

	mainWindow->add_action("save", Gio::ActionMap::ActivateSlot([&] {
		auto lock = lockGame();
		app->game->save();
	}));

	// mainWindow->add_action("save_as", Gio::ActionMap::ActivateSlot([&] {}));

	regionTab = std::make_unique<RegionTab>(*this);
	travelTab = std::make_unique<TravelTab>(*this);
	inventoryTab = std::make_unique<InventoryTab>(*this);

	notebook->append_page(regionTab->box, "Region");
	notebook->append_page(travelTab->grid, "Travel");
	notebook->append_page(inventoryTab->scrolled, "Inventory");
}

void App::quit() {
	gtkApp->quit();
}

void App::delay(std::function<void()> fn) {
	mainWindow->add_tick_callback([fn](const auto &) {
		fn();
		return false;
	});
}

void App::alert(const Glib::ustring &message, Gtk::MessageType type, bool modal, bool use_markup) {
	dialog.reset(new Gtk::MessageDialog(*mainWindow, message, use_markup, type, Gtk::ButtonsType::OK, modal));
	dialog->signal_response().connect([this](int) {
		dialog->close();
	});
	dialog->show();
}

void App::error(const Glib::ustring &message, bool modal, bool use_markup) {
	alert(message, Gtk::MessageType::ERROR, modal, use_markup);
}

int App::run(int argc, char **argv) {
	gtkApp->signal_activate().connect([this] {
		gtkApp->add_window(*mainWindow);
		mainWindow->show();
	});
	return gtkApp->run(argc, argv);
}

void App::onTravel() {
	regionTab->update();
	travelTab->update();
}

void App::updateDialog() {
	if (dialog)
		if (auto *udialog = dynamic_cast<UpdatingDialog *>(dialog.get()))
			udialog->updateData();
}
