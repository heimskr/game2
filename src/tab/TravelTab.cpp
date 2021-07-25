#include "Game.h"
#include "UI.h"
#include "tab/TravelTab.h"
#include "ui/AppWindow.h"

namespace Game2 {
	TravelTab::TravelTab(AppWindow &app_window): appWindow(app_window) {
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
					std::shared_ptr<Region> region = appWindow.game->currentRegionPointer();
					if (!region)
						return;
					const Position pos = getPosition(*region, row, column);
					if (appWindow.game->regions.count(pos) != 0)
						appWindow.game->position = pos;
					else
						*appWindow.game += Region::generate(*appWindow.game, pos);
					appWindow.onTravel();
				});
			}
		reset();
	}

	void TravelTab::reset() {
		if (!appWindow.game)
			return;

		auto lock = appWindow.lockGame();

		auto region = appWindow.game->currentRegionPointer();
		if (!region)
			return;

		for (int row = 0; row < ROWS; ++row)
			for (int column = 0; column < COLUMNS; ++column) {
				Gtk::Button &button = buttons[row * COLUMNS + column];
				button.set_label("");
				if (region) {
					const Position pos = getPosition(*region, row, column);
					if (appWindow.game->regions.count(pos) != 0) {
						auto &region = appWindow.game->regions.at(pos);
						button.set_label(region->name);
						if (region->occupied())
							button.add_css_class("bold");
						else
							button.remove_css_class("bold");
					}
				}
			}
	}

	Position TravelTab::getPosition(Region &region, int row, int column) {
		return region.position + Position(column - COLUMNS / 2, row - ROWS / 2);
	}
}
