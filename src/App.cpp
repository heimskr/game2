#include <iostream>

#include "App.h"
#include "UI.h"

std::unique_ptr<App> app;

void App::quit() {
	gtkApp->quit();
}

void App::updateRegion() {
	auto lock = lockGame();

	auto region_ptr = game->currentRegionPointer();
	auto &region = *region_ptr;
	auto &pos = region.position;

	{
		Gtk::Label *label = getWidget<Gtk::Label>("region_name");
		label->set_text(region.name);

		label = getWidget<Gtk::Label>("position_label");
		label->set_text("Position: (" + std::to_string(pos.first) + ", " + std::to_string(pos.second) + ")");

		label = getWidget<Gtk::Label>("size_label");
		label->set_text("Size: " + std::to_string(region.size));
	}

	if (region_ptr != lastRegion) {
		lastRegion = region_ptr;
		auto *box = getWidget<Gtk::Box>("regions_box");
		removeChildren(*box);
		areaWidgets.clear();
		for (auto &[name, area]: region.areas) {
			Gtk::Expander *expander = new Gtk::Expander(area->name);
			areaWidgets.emplace_back(expander);
			box->add(*expander);
			expander->set_margin_bottom(10);

			Gtk::Box *ebox = new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL, 5);
			areaWidgets.emplace_back(ebox);
			expander->add(*ebox);

			for (const auto &[rname, amount]: area->resources) {
				Gtk::Box *rbox = new Gtk::Box(Gtk::Orientation::ORIENTATION_HORIZONTAL, 5);
				areaWidgets.emplace_back(rbox);
				ebox->add(*rbox);

				Gtk::Button *button = new Gtk::Button("Extract");
				areaWidgets.emplace_back(button);
				rbox->add(*button);

				Gtk::Label *label = new Gtk::Label(rname + " x " + std::to_string(amount));
				areaWidgets.emplace_back(label);
				label->set_halign(Gtk::Align::ALIGN_START);
				label->set_hexpand(true);
				rbox->add(*label);

				button = new Gtk::Button("Move");
				areaWidgets.emplace_back(button);
				button->set_tooltip_text("Move a resource from your inventory into the area");
				rbox->add(*button);

				button = new Gtk::Button("Resize");
				areaWidgets.emplace_back(button);
				rbox->add(*button);
			}
		}
		box->show_all_children();
	}
}

void App::updateTravel() {
	auto lock = lockGame();

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
					updateRegion();
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
