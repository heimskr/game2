#include <iostream>

#include "App.h"
#include "UI.h"
#include "ui/InventoryDialog.h"

std::unique_ptr<App> app;

void App::quit() {
	gtkApp->quit();
}

void App::updateTravel() {
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
	auto dialock = lockDialog();
	if (dialog)
		if (auto *udialog = dynamic_cast<UpdatingDialog *>(dialog.get()))
			udialog->updateData();
}

Region::Position App::getPosition(Region &region, int row, int column) {
	return {region.position.first + column - COLUMNS / 2, region.position.second + row - ROWS / 2};
}
