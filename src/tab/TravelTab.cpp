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
		reset();
	}

	void TravelTab::onFocus() {
		zoomOutButton = std::make_unique<Gtk::Button>();
		zoomOutButton->set_icon_name("zoom-out-symbolic");
		zoomOutButton->set_tooltip_text("Zoom out");

		zoomInButton = std::make_unique<Gtk::Button>();
		zoomInButton->set_icon_name("zoom-in-symbolic");
		zoomInButton->set_tooltip_text("Zoom in");

		zoomOutButton->signal_clicked().connect([this] {
			rows += 2;
			columns += 2;
			reset();
		});

		zoomInButton->signal_clicked().connect([this] {
			if (1 < rows) {
				rows -= 2;
				columns -= 2;
				reset();
			}
		});

		appWindow.header->pack_start(*zoomOutButton);
		appWindow.header->pack_start(*zoomInButton);
		appWindow.titleWidgets.push_back(zoomOutButton.get());
		appWindow.titleWidgets.push_back(zoomInButton.get());
	}

	void TravelTab::onBlur() {
		zoomOutButton.reset();
		zoomInButton.reset();
	}

	void TravelTab::reset() {
		if (!appWindow.game)
			return;

		auto lock = appWindow.lockGame();

		auto region = appWindow.game->currentRegionPointer();
		if (!region)
			return;

		for (size_t i = 0; i < buttonsLength; ++i)
			grid.remove(buttons[i]);

		buttons = std::make_unique<Gtk::Button[]>(buttonsLength = buttonCount());

		for (int row = 0; row < rows; ++row)
			for (int column = 0; column < columns; ++column) {
				Gtk::Button &button = buttons[row * columns + column];
				button.set_label("");
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
		return region.position + Position(column - columns / 2, row - rows / 2);
	}
}
