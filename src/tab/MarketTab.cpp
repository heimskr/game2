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
		sellGrid.set_row_spacing(5);
		sellGrid.set_column_spacing(5);
		gridBox.append(sellGrid);
		buyGrid.set_row_spacing(5);
		buyGrid.set_column_spacing(5);
		gridBox.append(buyGrid);
		scrolled.set_vexpand(true);
		box.append(scrolled);
		setMargins(box, 5);
		reset();
	}

	void MarketTab::reset() {
		removeChildren(sellGrid);
		removeChildren(buyGrid);
		sellWidgets.clear();
		buyWidgets.clear();

		updateMoney();
		addSellHeader();
		addBuyHeader();
		resetSell();
		resetBuy();
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

	void MarketTab::addSellHeader() {
		auto *label = new Gtk::Label("Your Resources", Gtk::Align::START);
		sellWidgets.emplace_back(label);
		label->add_css_class("table-header");
		label->set_hexpand(true);
		sellGrid.attach(*label, 1, 0);
		label = new Gtk::Label("Amount", Gtk::Align::START);
		sellWidgets.emplace_back(label);
		label->add_css_class("table-header");
		sellGrid.attach(*label, 2, 0);
		label = new Gtk::Label("Price", Gtk::Align::START);
		sellWidgets.emplace_back(label);
		label->add_css_class("table-header");
		sellGrid.attach(*label, 3, 0);
	}

	void MarketTab::addBuyHeader() {
		auto *label = new Gtk::Label("Region Resources", Gtk::Align::START);
		buyWidgets.emplace_back(label);
		label->add_css_class("table-header");
		label->set_hexpand(true);
		buyGrid.attach(*label, 0, 0);
		label = new Gtk::Label("Amount", Gtk::Align::START);
		buyWidgets.emplace_back(label);
		label->add_css_class("table-header");
		buyGrid.attach(*label, 1, 0);
		label = new Gtk::Label("Price", Gtk::Align::START);
		buyWidgets.emplace_back(label);
		label->add_css_class("table-header");
		buyGrid.attach(*label, 2, 0);
	}

	void MarketTab::resetSell() {
		auto lock = app.lockGame();
		if (!app.game)
			return;

		removeChildren(sellGrid);
		sellWidgets.clear();
		sellAmountLabels.clear();
		sellPriceLabels.clear();
		addSellHeader();

		Region &region = app.game->currentRegion();
		auto non_owned = region.allNonOwnedResources();
		double money = app.game->money;
		double greed = region.greed;
		int row = 1;
		previousInventory.clear();

		for (const auto &[name, amount]: app.game->inventory) {
			previousInventory.insert(name);
			auto *button = new Gtk::Button;
			sellWidgets.emplace_back(button);
			button->set_icon_name("list-remove-symbolic");
			button->set_tooltip_text("Sell resource");
			button->signal_clicked().connect([this, name] { sell(name); });
			sellGrid.attach(*button, 0, row);

			auto *label = new Gtk::Label(name, Gtk::Align::START);
			sellWidgets.emplace_back(label);
			sellGrid.attach(*label, 1, row);

			label = &sellAmountLabels.emplace(name, Gtk::Label(niceDouble(amount), Gtk::Align::START)).first->second;
			sellGrid.attach(*label, 2, row);

			label = &sellPriceLabels.emplace(name, Gtk::Label(niceDouble(
				Stonks::sellPrice(app.game->resources.at(name).basePrice, non_owned.count(name)? non_owned.at(name) : 0,
				money, greed)), Gtk::Align::START)).first->second;
			sellGrid.attach(*label, 3, row);

			++row;
		}
	}

	void MarketTab::resetBuy() {
		auto lock = app.lockGame();
		if (!app.game)
			return;

		removeChildren(buyGrid);
		buyWidgets.clear();
		buyAmountLabels.clear();
		buyPriceLabels.clear();
		addBuyHeader();

		Region &region = app.game->currentRegion();
		auto non_owned = region.allNonOwnedResources();
		double money = app.game->money;
		previousNonOwned.clear();

		int row = 1;

		for (const auto &[name, amount]: non_owned) {
			previousNonOwned.insert(name);
			auto *label = new Gtk::Label(name, Gtk::Align::START);
			buyWidgets.emplace_back(label);
			buyGrid.attach(*label, 0, row);

			label = &buyAmountLabels.emplace(name, Gtk::Label(niceDouble(amount), Gtk::Align::START)).first->second;
			buyGrid.attach(*label, 1, row);

			label = &buyPriceLabels.emplace(name, Gtk::Label(niceDouble(Stonks::buyPrice(
				app.game->resources.at(name).basePrice, amount, money)),Gtk::Align::START)).first->second;
			buyGrid.attach(*label, 2, row);

			auto *button = new Gtk::Button;
			buyWidgets.emplace_back(button);
			button->set_icon_name("list-add-symbolic");
			button->set_tooltip_text("Buy resource");
			button->signal_clicked().connect([this, name] { buy(name); });
			buyGrid.attach(*button, 3, row);

			++row;
		}
	}

	void MarketTab::updateSell() {
		auto lock = app.lockGame();
		if (!app.game)
			return;

		Region &region = app.game->currentRegion();
		auto non_owned = region.allNonOwnedResources();
		double money = app.game->money;
		double greed = region.greed;

		if (!compare(previousInventory, app.game->inventory)) {
			resetSell();
		} else {
			for (const auto &[name, amount]: app.game->inventory) {
				if (sellAmountLabels.count(name) != 0) {
					const Glib::ustring string = niceDouble(amount);
					auto &label = sellAmountLabels.at(name);
					if (label.get_text() != string)
						label.set_text(string);
				}

				if (sellPriceLabels.count(name) != 0) {
					const Glib::ustring string = niceDouble(Stonks::sellPrice(app.game->resources.at(name).basePrice,
						non_owned.count(name)? non_owned.at(name) : 0, money, greed));
					auto &label = sellPriceLabels.at(name);
					if (label.get_text() != string)
						label.set_text(string);
				}
			}
		}
	}

	void MarketTab::updateBuy() {
		auto lock = app.lockGame();
		if (!app.game)
			return;

		Region &region = app.game->currentRegion();
		auto non_owned = region.allNonOwnedResources();
		double money = app.game->money;

		if (!compare(previousNonOwned, non_owned)) {
			resetBuy();
		} else {
			for (const auto &[name, amount]: non_owned) {
				if (sellAmountLabels.count(name) != 0) {
					const Glib::ustring string = niceDouble(amount);
					auto &label = buyAmountLabels.at(name);
					if (label.get_text() != string)
						label.set_text(string);
				}

				if (sellPriceLabels.count(name) != 0) {
					const Glib::ustring string = niceDouble(Stonks::buyPrice(app.game->resources.at(name).basePrice,
						amount, money));
					auto &label = buyPriceLabels.at(name);
					if (label.get_text() != string)
						label.set_text(string);
				}
			}
		}
	}

	void MarketTab::update() {
		updateSell();
		updateBuy();
	}

	void MarketTab::sell(const std::string &resource_name) {
		std::cout << "Sell " << resource_name << "\n";
	}

	void MarketTab::buy(const std::string &resource_name) {
		std::cout << "Buy " << resource_name << "\n";
	}
}