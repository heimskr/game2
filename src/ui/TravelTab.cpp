#include "App.h"
#include "UI.h"
#include "ui/TravelTab.h"

namespace Game2 {
	TravelTab::TravelTab(App &app_): app(app_) {
		grid.set_row_homogeneous(true);
		grid.set_column_homogeneous(true);
		grid.set_row_spacing(5);
		grid.set_column_spacing(5);
		setMargins(grid, 5);
		for (int row = 0; row < ROWS; ++row)
			for (int column = 0; column < COLUMNS; ++column) {
				Gtk::Button &button = buttons[row * COLUMNS + column];
				grid.attach(button, column, row);
				button.signal_clicked().connect([this, row, column] {
					std::shared_ptr<Region> region = app.game->currentRegionPointer();
					if (!region)
						return;
					const Region::Position pos = getPosition(*region, row, column);
					if (app.game->regions.count(pos) != 0)
						app.game->position = pos;
					else
						*app.game += Region::generate(*app.game, pos);
					update();
					app.onTravel();
				});
			}
		update();
	}

	void TravelTab::update() {
		if (!app.game)
			return;

		auto lock = app.lockGame();

		auto region = app.game->currentRegionPointer();
		if (!region)
			return;

		for (int row = 0; row < ROWS; ++row)
			for (int column = 0; column < COLUMNS; ++column) {
				Gtk::Button &button = buttons[row * COLUMNS + column];
				button.set_label("");
				if (region) {
					const Region::Position pos = getPosition(*region, row, column);
					if (app.game->regions.count(pos) != 0)
						button.set_label(app.game->regions.at(pos)->name);
				}
			}
	}

	Region::Position TravelTab::getPosition(Region &region, int row, int column) {
		return region.position + Region::Position(column - COLUMNS / 2, row - ROWS / 2);
	}
}
