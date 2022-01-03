#include "UI.h"
#include "tab/ExtractionsTab.h"
#include "tab/RegionTab.h"
#include "ui/AppWindow.h"

namespace Game2 {
	ExtractionsTab::ExtractionsTab(AppWindow &app_window): appWindow(app_window) {
		scrolled.set_vexpand(true);
		scrolled.set_child(treeView);
		treeModel = Gtk::ListStore::create(columns);
		treeView.set_model(treeModel);
		appendColumn(treeView, "Resource", columns.resource);
		appendColumn(treeView, "Area", columns.area);
		appendColumn(treeView, "Region", columns.region);
		appendColumn(treeView, "Rate", columns.rate);
		for (int i = 0, columns = treeView.get_n_columns(); i < columns; ++i) {
			auto *column = treeView.get_column(i);
			column->set_expand(true);
			column->set_resizable(true);
		}
	}

	void ExtractionsTab::onFocus() {
		if (!appWindow.game)
			return;

		globalButton = std::make_unique<Gtk::ToggleButton>("Global");
		globalButton->set_active(global);
		globalButton->signal_clicked().connect(sigc::mem_fun(*this, &ExtractionsTab::toggleGlobal));
		appWindow.header->pack_start(*globalButton);
		appWindow.titleWidgets.push_back(globalButton.get());

		removeButton = std::make_unique<Gtk::Button>();
		removeButton->set_icon_name("list-remove-symbolic");
		removeButton->signal_clicked().connect(sigc::mem_fun(*this, &ExtractionsTab::removeExtraction));
		appWindow.header->pack_start(*removeButton);
		appWindow.titleWidgets.push_back(removeButton.get());
	}

	void ExtractionsTab::onBlur() {
		globalButton.reset();
		removeButton.reset();
	}

	void ExtractionsTab::reset() {
		treeModel->clear();

		if (!appWindow.game)
			return;

		auto lock = appWindow.lockGame();
		auto region = appWindow.game->currentRegionPointer();
		auto &extractions = appWindow.game->extractions;

		for (auto iter = extractions.begin(), end = extractions.end(); iter != end; ++iter) {
			const Extraction &extraction = *iter;

			if (!global && extraction.area->parent != region.get())
				continue;

			auto row = *treeModel->append();
			row[columns.resource] = extraction.resourceName;
			row[columns.area] = extraction.area->name;
			row[columns.region] = extraction.area->parent->name;
			row[columns.rate] = niceDouble(extraction.rate) + "/s";
			row[columns.iter] = iter;
		}
	}

	void ExtractionsTab::toggleGlobal() {
		globalButton->set_active(global = !global);
		reset();
	}

	void ExtractionsTab::removeExtraction() {
		if (auto iter = treeView.get_selection()->get_selected()) {
			auto extraction_iter = iter->get_value(columns.iter);
			appWindow.game->extractions.erase(extraction_iter);
			appWindow.regionTab->reset();
			treeModel->erase(iter);
		}
	}
}
