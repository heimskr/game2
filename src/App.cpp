#include <iostream>

#include "App.h"

std::unique_ptr<App> app;

void App::quit() {
	gtkApp->quit();
}

void App::updateRegion() {
	auto *label = getWidget<Gtk::Label>("region_name");
	label->set_text(game->currentRegion().name);
}

void App::updateTravel() {
	// constexpr int ROWS = 5, COLUMNS = 5;

	auto *grid = getWidget<Gtk::Grid>("travel_grid");

	if (travelButtons.empty())
		for (int row = 0; row < ROWS; ++row)
			for (int column = 0; column < COLUMNS; ++column) {
				Gtk::Button &button = travelButtons.emplace_back("");
				button.set_can_focus(false);
				button.set_halign(Gtk::Align::ALIGN_FILL);
				button.set_valign(Gtk::Align::ALIGN_FILL);
				button.property_expand().set_value(true);
				button.signal_clicked().connect([this, row, column] {
					std::shared_ptr<Region> region = game->currentRegionPointer();
					if (!region)
						return;
					const Region::Position pos = getPosition(*region, row, column);
					if (game->regions.count(pos) != 0)
						game->position = pos;
					else
						*game += Region::generate(*game, pos);
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
	grid->show_all_children();
}



Region::Position App::getPosition(Region &region, int row, int column) {
	return {region.position.first + column - COLUMNS / 2, region.position.second + row - ROWS / 2};
}
