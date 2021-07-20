#include <iostream>

#include "App.h"
#include "Stonks.h"
#include "UI.h"
#include "Util.h"
#include "area/Housing.h"
#include "tab/MarketTab.h"
#include "ui/EntryDialog.h"
#include "ui/NumericEntry.h"

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

		auto lock = app.lockGame();
		if (!app.game)
			return;

		std::shared_ptr<HousingArea> housing;
		if (auto region = app.game->currentRegionPointer())
			housing = region->getHousing();

		if (!housing) {
			scrolled.set_child(errorLabel);
			regionMoneyLabel.hide();
			regionMoney.hide();
			return;
		}

		scrolled.set_child(gridBox);
		regionMoneyLabel.show();
		regionMoney.show();

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
		label->set_xalign(0);
		label->set_size_request(100, -1);
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
		label->set_xalign(0);
		label->set_size_request(100, -1);
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
				if (buyAmountLabels.count(name) != 0) {
					const Glib::ustring string = niceDouble(amount);
					auto &label = buyAmountLabels.at(name);
					if (label.get_text() != string)
						label.set_text(string);
				}

				if (buyPriceLabels.count(name) != 0) {
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
		auto *dialog = new EntryDialog<NumericEntry>("Sell", *app.mainWindow, "Amount to sell:");
		app.dialog.reset(dialog);
		dialog->signal_submit().connect([this, resource_name](const Glib::ustring &response) {
			app.delay([this, resource_name, response] {
				double amount;
				try {
					amount = parseDouble(response);
				} catch (std::invalid_argument &) {
					app.error("Invalid amount.");
					return;
				}

				app.gameMutex.lock();
				auto region = app.game->currentRegionPointer();
				double &in_inventory = app.game->inventory[resource_name];

				if (lte(amount, 0) || ltna(in_inventory, amount)) {
					app.gameMutex.unlock();
					app.error("Invalid amount.");
					return;
				}

				size_t total_price;
				if (!Stonks::totalSellPrice(*region, resource_name, amount, total_price)) {
					app.gameMutex.unlock();
					app.error("Region doesn't have enough money. Price: " + std::to_string(total_price));
				} else {
					auto *dialog = new Gtk::MessageDialog(*app.mainWindow, "Price: " + std::to_string(total_price),
						false, Gtk::MessageType::QUESTION, Gtk::ButtonsType::OK_CANCEL, true);
					app.dialog.reset(dialog);
					dialog->signal_response().connect([this, resource_name, region, amount, total_price](int response) {
						if (response == Gtk::ResponseType::OK) {
							Region &region = app.game->currentRegion();
							auto housing = region.getHousing();
							if (!housing) {
								app.error("Region has no market.");
							} else {
								housing->resources[resource_name] += amount;
								app.game->inventory[resource_name] -= amount;
								region.setMoney(region.money - total_price);
								app.game->setMoney(app.game->money + total_price);
								shrink(app.game->inventory);
							}
						}
						app.gameMutex.unlock();
						app.dialog->hide();
					});
					app.dialog->show();
				}
			});
		});
		app.dialog->show();
	}

	void MarketTab::buy(const std::string &resource_name) {
		auto *dialog = new EntryDialog<NumericEntry>("Buy", *app.mainWindow, "Amount to buy:");
		app.dialog.reset(dialog);
		dialog->signal_submit().connect([this, resource_name](const Glib::ustring &response) {
			app.delay([this, resource_name, response] {
				double amount;
				try {
					amount = parseDouble(response);
				} catch (std::invalid_argument &) {
					app.error("Invalid amount.");
					return;
				}

				app.gameMutex.lock();
				auto region = app.game->currentRegionPointer();
				double in_region = region->allNonOwnedResources()[resource_name];

				if (lte(amount, 0) || ltna(in_region, amount)) {
					app.gameMutex.unlock();
					app.error("Invalid amount.");
					return;
				}

				const size_t total_price = Stonks::totalBuyPrice(*region, resource_name, amount);
				if (app.game->money < total_price) {
					app.error("You don't have enough money.");
					app.gameMutex.unlock();
					return;
				}

				auto *dialog = new Gtk::MessageDialog(*app.mainWindow, "Amount: " + std::to_string(total_price), false,
					Gtk::MessageType::QUESTION, Gtk::ButtonsType::OK_CANCEL, true);
				app.dialog.reset(dialog);
				dialog->signal_response().connect([this, resource_name, total_price, amount, region](int response) {
					if (response == Gtk::ResponseType::OK) {
						region->subtractResourceFromNonOwned(resource_name, amount);
						app.game->addToInventory(resource_name, amount);
						region->setMoney(region->money + total_price);
						app.game->setMoney(app.game->money - total_price);
						for (auto &[name, area]: region->areas)
							shrink(area->resources);
					}
					app.gameMutex.unlock();
					app.dialog->hide();
				});
				app.dialog->show();
			});
		});
		app.dialog->show();
	}
}
