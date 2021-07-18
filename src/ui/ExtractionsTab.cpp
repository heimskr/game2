#include "App.h"
#include "UI.h"
#include "ui/ExtractionsTab.h"

ExtractionsTab::ExtractionsTab(App &app_): app(app_) {
	scrolled.set_child(grid);
	grid.set_row_spacing(5);
	grid.set_column_spacing(15);
	setMargins(grid, 5);
}

void ExtractionsTab::reset() {
	removeChildren(grid);
	widgets.clear();

	auto lock = app.lockGame();
	if (!app.game)
		return;

	auto region = app.game->currentRegionPointer();
	
	int row = 0;
	for (const Extraction &extraction: app.game->extractions) {
		if (extraction.area->parent != region.get())
			continue;

		auto *button = new Gtk::Button;
		widgets.emplace_back(button);
		button->set_icon_name("list-remove-symbolic");
		grid.attach(*button, 0, row);
		
		auto *label = new Gtk::Label(extraction.resourceName);
		widgets.emplace_back(label);
		label->set_halign(Gtk::Align::START);
		grid.attach(*label, 1, row);

		label = new Gtk::Label(extraction.area->name);
		widgets.emplace_back(label);
		label->set_halign(Gtk::Align::START);
		grid.attach(*label, 2, row);

		label = new Gtk::Label(std::to_string(extraction.rate) + "/s");
		widgets.emplace_back(label);
		label->set_halign(Gtk::Align::START);
		grid.attach(*label, 3, row);

		++row;
	}
}
