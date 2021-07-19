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

	void ExtractionsTab::onFocus() {
		if (!app.game)
			return;

		globalButton = std::make_unique<Gtk::ToggleButton>("Global");
		globalButton->set_active(global);
		globalButton->signal_clicked().connect([this] {
			globalButton->set_active(global = !global);
			reset();
		});

		app.header->pack_start(*globalButton);
		app.titleWidgets.push_back(globalButton.get());
	}

	void ExtractionsTab::onBlur() {
		globalButton.reset();
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

			if (!global && extraction.area->parent != region.get())
				continue;

			auto &button = static_cast<Gtk::Button &>(*widgets.emplace_back(new Gtk::Button));
			button.set_tooltip_text("Remove extraction");
			button.set_icon_name("list-remove-symbolic");
			button.signal_clicked().connect([this, iter] {
				app.game->extractions.erase(iter);
				reset();
			});
			grid.attach(button, 0, row);

			auto *label = new Gtk::Label(extraction.resourceName, Gtk::Align::START);
			widgets.emplace_back(label);
			grid.attach(*label, 1, row);

			label = new Gtk::Label(extraction.area->name, Gtk::Align::START);
			widgets.emplace_back(label);
			grid.attach(*label, 2, row);

			if (global) {
				label = new Gtk::Label(extraction.area->parent->name, Gtk::Align::START);
				widgets.emplace_back(label);
				grid.attach(*label, 3, row);
			}

			label = new Gtk::Label(niceDouble(extraction.rate) + "/s", Gtk::Align::START);
			widgets.emplace_back(label);
			grid.attach(*label, global? 4 : 3, row);

			++row;
		}
	}
}
