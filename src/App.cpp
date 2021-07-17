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
		app->regionTab->update();
		app->updateTravel();
	}));

	mainWindow->add_action("open", Gio::ActionMap::ActivateSlot([this] {
		notebook->show();
		auto lock = lockGame();
		app->game = Game::load();
		app->regionTab->update();
		app->updateTravel();
	}));

	mainWindow->add_action("save", Gio::ActionMap::ActivateSlot([&] {
		auto lock = lockGame();
		app->game->save();
	}));

	// mainWindow->add_action("save_as", Gio::ActionMap::ActivateSlot([&] {}));

	regionTab = std::make_unique<RegionTab>(*this);
	travelTab = std::make_unique<TravelTab>(*this);

	notebook->append_page(regionTab->box, "Region");
	notebook->append_page(travelTab->grid, "Travel");
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

void App::updateTravel() {
	return;
	auto lock = lockGame();

	auto *grid = builder->get_widget<Gtk::Grid>("travel_grid");

	if (travelButtons.empty())
		for (int row = 0; row < ROWS; ++row)
			for (int column = 0; column < COLUMNS; ++column) {
				Gtk::Button &button = travelButtons.emplace_back("");
				button.set_can_focus(false);
				button.set_halign(Gtk::Align::FILL);
				button.set_valign(Gtk::Align::FILL);
				button.set_expand(true);
				button.signal_clicked().connect([this, row, column] {
					std::shared_ptr<Region> region = game->currentRegionPointer();
					if (!region)
						return;
					const Region::Position pos = getPosition(*region, row, column);
					if (game->regions.count(pos) != 0)
						game->position = pos;
					else
						*game += Region::generate(*game, pos);
					regionTab->update();
					updateTravel();
				});
				grid->attach(button, column, row);
			}

	auto region = game->currentRegionPointer();

	for (int row = 0; row < ROWS; ++row)
		for (int column = 0; column < COLUMNS; ++column) {
			Gtk::Button &button = travelButtons.at(row * COLUMNS + column);
			button.set_label("");
			if (region) {
				const Region::Position pos = getPosition(*region, row, column);
				if (game->regions.count(pos) != 0)
					button.set_label(game->regions.at(pos)->name);
			}
		}

	setMargins(*grid, 5);
	grid->set_row_spacing(5);
	grid->set_column_spacing(5);
}

void App::updateDialog() {
	if (dialog)
		if (auto *udialog = dynamic_cast<UpdatingDialog *>(dialog.get()))
			udialog->updateData();
}

Region::Position App::getPosition(Region &region, int row, int column) {
	return {region.position.first + column - COLUMNS / 2, region.position.second + row - ROWS / 2};
}
