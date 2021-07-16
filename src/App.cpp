#include "App.h"

std::unique_ptr<App> app;

void App::quit() {
	gtkApp->quit();
}

void App::updateRegion() {
	auto *label = getWidget<Gtk::Label>("region_name");
	label->set_text(game.currentRegion().name);
}

void App::updateTravel() {
	auto *grid = getWidget<Gtk::Grid>("travel_grid");
	for (Gtk::Button &button: travelButtons)
		grid->remove(button);

	travelButtons.clear();

	for (int row = 0; row < 5; ++row) {
		for (int column = 0; column < 5; ++column) {
			travelButtons.emplace_back(std::to_string(row) + "," + std::to_string(column));
			Gtk::Button &button = travelButtons.back();
			grid->attach(button, column, row);
			button.property_expand().set_value(true);
		}
	}

	setMargins(*grid, 5);
	grid->set_row_spacing(5);
	grid->set_column_spacing(5);
	grid->show_all_children();
}
