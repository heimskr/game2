#include "App.h"
#include "UI.h"
#include "Util.h"
#include "tab/MarketTab.h"

namespace Game2 {
	MarketTab::MarketTab(App &app_): app(app_) {
		regionMoneyLabel.set_halign(Gtk::Align::END);
		topGrid.attach(regionMoneyLabel, 0, 0);
		regionMoney.set_halign(Gtk::Align::START);
		topGrid.attach(regionMoney, 1, 0);
		yourMoneyLabel.set_halign(Gtk::Align::END);
		topGrid.attach(yourMoneyLabel, 0, 1);
		yourMoney.set_halign(Gtk::Align::START);
		topGrid.attach(yourMoney, 1, 1);
		topGrid.set_row_spacing(5);
		topGrid.set_column_spacing(10);
		box.append(topGrid);
		mainGrid.set_row_spacing(5);
		mainGrid.set_column_spacing(5);
		scrolled.set_child(mainGrid);
		scrolled.set_vexpand(true);
		box.append(scrolled);
		setMargins(box, 5);
		reset();
	}

	void MarketTab::reset() {
		removeChildren(mainGrid);
		widgets.clear();

		updateMoney();
		addHeader();
	}

	void MarketTab::updateMoney() {
		auto lock = app.lockGame();
		if (!app.game)
			return;

		auto region = app.game->currentRegionPointer();
		if (region)
			regionMoney.set_label(niceDouble(region->money));

		yourMoney.set_label(niceDouble(app.game->money));
	}

	void MarketTab::addHeader() {
		auto *label = new Gtk::Label("Your Resources", Gtk::Align::START);
		widgets.emplace_back(label);
		label->add_css_class("table-header");
		label->set_hexpand(true);
		mainGrid.attach(*label, 1, 0);
		label = new Gtk::Label("Amount", Gtk::Align::START);
		widgets.emplace_back(label);
		label->add_css_class("table-header");
		mainGrid.attach(*label, 2, 0);
		label = new Gtk::Label("Price", Gtk::Align::START);
		widgets.emplace_back(label);
		label->add_css_class("table-header");
		mainGrid.attach(*label, 3, 0);
		label = new Gtk::Label("Region Resources", Gtk::Align::START);
		widgets.emplace_back(label);
		label->add_css_class("table-header");
		label->set_hexpand(true);
		mainGrid.attach(*label, 4, 0);
		label = new Gtk::Label("Amount", Gtk::Align::START);
		widgets.emplace_back(label);
		label->add_css_class("table-header");
		mainGrid.attach(*label, 5, 0);
		label = new Gtk::Label("Price", Gtk::Align::START);
		widgets.emplace_back(label);
		label->add_css_class("table-header");
		mainGrid.attach(*label, 6, 0);
	}
}
