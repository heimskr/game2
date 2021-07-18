#include <iostream>
#include "App.h"
#include "UI.h"
#include "tab/ExtractionsTab.h"

namespace Game2 {
	ExtractionsTab::ExtractionsTab(App &app_): app(app_) {
		scrolled.set_child(grid);
		setMargins(grid, 5);
		grid.set_row_spacing(5);
		grid.set_column_spacing(15);
	}

	void ExtractionsTab::reset() {
		removeChildren(grid);
		widgets.clear();

		auto lock = app.lockGame();
		if (!app.game)
			return;

		auto region = app.game->currentRegionPointer();
		int row = 0;

		for (auto iter = app.game->extractions.begin(), end = app.game->extractions.end(); iter != end; ++iter) {
			const Extraction &extraction = *iter;

			if (extraction.area->parent != region.get())
				continue;

			auto &button = static_cast<Gtk::Button &>(*widgets.emplace_back(new Gtk::Button));
			button.set_tooltip_text("Remove extraction");
			button.set_icon_name("list-remove-symbolic");
			button.signal_clicked().connect([this, iter] {
				app.game->extractions.erase(iter);
				reset();
			});
			grid.attach(button, 0, row);

			auto *label = new Gtk::Label(extraction.resourceName);
			widgets.emplace_back(label);
			label->set_halign(Gtk::Align::START);
			grid.attach(*label, 1, row);

			label = new Gtk::Label(extraction.area->name);
			widgets.emplace_back(label);
			label->set_halign(Gtk::Align::START);
			grid.attach(*label, 2, row);

			label = new Gtk::Label(niceDouble(extraction.rate) + "/s");
			widgets.emplace_back(label);
			label->set_halign(Gtk::Align::START);
			grid.attach(*label, 3, row);

			++row;
		}
	}
}
