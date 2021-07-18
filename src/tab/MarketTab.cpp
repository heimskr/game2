#include <iostream>

#include "App.h"
#include "UI.h"
#include "Util.h"
#include "Stonks.h"
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
		gridBox.set_homogeneous(true);
		gridBox.set_spacing(50);
		scrolled.set_child(gridBox);
		leftGrid.set_row_spacing(5);
		leftGrid.set_column_spacing(5);
		gridBox.append(leftGrid);
		rightGrid.set_row_spacing(5);
		rightGrid.set_column_spacing(5);
		gridBox.append(rightGrid);
		scrolled.set_vexpand(true);
		box.append(scrolled);
		setMargins(box, 5);
		reset();
	}

	void MarketTab::reset() {
		removeChildren(leftGrid);
		removeChildren(rightGrid);
		widgets.clear();

		updateMoney();
		addHeader();
		resetGrids();
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
		leftGrid.attach(*label, 1, 0);
		label = new Gtk::Label("Amount", Gtk::Align::START);
		widgets.emplace_back(label);
		label->add_css_class("table-header");
		leftGrid.attach(*label, 2, 0);
		label = new Gtk::Label("Price", Gtk::Align::START);
		widgets.emplace_back(label);
		label->add_css_class("table-header");
		leftGrid.attach(*label, 3, 0);
		label = new Gtk::Label("Region Resources", Gtk::Align::START);
		widgets.emplace_back(label);
		label->add_css_class("table-header");
		label->set_hexpand(true);
		rightGrid.attach(*label, 0, 0);
		label = new Gtk::Label("Amount", Gtk::Align::START);
		widgets.emplace_back(label);
		label->add_css_class("table-header");
		rightGrid.attach(*label, 1, 0);
		label = new Gtk::Label("Price", Gtk::Align::START);
		widgets.emplace_back(label);
		label->add_css_class("table-header");
		rightGrid.attach(*label, 2, 0);
	}

	void MarketTab::resetGrids() {
		auto lock = app.lockGame();
		if (!app.game)
			return;

		Region &region = app.game->currentRegion();
		auto non_owned = region.allNonOwnedResources();
		double money = app.game->money;
		double greed = region.greed;

		int row = 1;
		for (const auto &[name, amount]: app.game->inventory) {
			auto *button = new Gtk::Button;
			widgets.emplace_back(button);
			button->set_icon_name("list-remove-symbolic");
			button->set_tooltip_text("Sell resource");
			button->signal_clicked().connect([this, name] { sell(name); });
			leftGrid.attach(*button, 0, row);

			auto *label = new Gtk::Label(name, Gtk::Align::START);
			widgets.emplace_back(label);
			leftGrid.attach(*label, 1, row);

			label = new Gtk::Label(niceDouble(amount), Gtk::Align::START);
			widgets.emplace_back(label);
			leftGrid.attach(*label, 2, row);

			label = new Gtk::Label(niceDouble(Stonks::sellPrice(app.game->resources.at(name).basePrice,
				non_owned.count(name)? non_owned.at(name) : 0, money, greed)), Gtk::Align::START);
			widgets.emplace_back(label);
			leftGrid.attach(*label, 3, row);

			++row;
		}

		row = 1;

		for (const auto &[name, amount]: non_owned) {
			auto *label = new Gtk::Label(name, Gtk::Align::START);
			widgets.emplace_back(label);
			rightGrid.attach(*label, 0, row);

			label = new Gtk::Label(niceDouble(amount), Gtk::Align::START);
			widgets.emplace_back(label);
			rightGrid.attach(*label, 1, row);

			label = new Gtk::Label(niceDouble(Stonks::buyPrice(app.game->resources.at(name).basePrice, amount, money)),
				Gtk::Align::START);
			widgets.emplace_back(label);
			rightGrid.attach(*label, 2, row);

			auto *button = new Gtk::Button;
			widgets.emplace_back(button);
			button->set_icon_name("list-add-symbolic");
			button->set_tooltip_text("Buy resource");
			button->signal_clicked().connect([this, name] { buy(name); });
			rightGrid.attach(*button, 3, row);

			++row;
		}
	}

	void MarketTab::sell(const std::string &resource_name) {
		std::cout << "Sell " << resource_name << "\n";
	}

	void MarketTab::buy(const std::string &resource_name) {
		std::cout << "Buy " << resource_name << "\n";
	}
}
