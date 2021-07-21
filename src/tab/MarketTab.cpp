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
		scrolled.set_child(tableBox);
		tableBox.set_homogeneous(true);
		tableBox.append(sellView);
		tableBox.append(buyView);
		scrolled.set_vexpand(true);
		box.append(scrolled);
		setMargins(topGrid, 5);

		sellModel = Gtk::ListStore::create(columns);
		sellView.set_model(sellModel);
		appendColumn(sellView, "Your Resources", columns.resource);
		appendColumn(sellView, "Amount", columns.amount);
		appendColumn(sellView, "Price", columns.price);

		buyModel = Gtk::ListStore::create(columns);
		buyView.set_model(buyModel);
		appendColumn(buyView, "Region Resources", columns.resource);
		appendColumn(buyView, "Amount", columns.amount);
		appendColumn(buyView, "Price", columns.price);

		for (auto *view: {&sellView, &buyView})
			for (int i = 0, columns = view->get_n_columns(); i < columns; ++i) {
				auto *column = view->get_column(i);
				column->set_expand(true);
				column->set_resizable(true);
			}

		reset();
	}

	void MarketTab::reset() {
		buyModel->clear();
		sellModel->clear();
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

		scrolled.set_child(tableBox);
		regionMoneyLabel.show();
		regionMoney.show();

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

	void MarketTab::resetSell() {
		sellModel->clear();
		sellRows.clear();

		if (!app.game)
			return;

		auto lock = app.lockGame();
		Region &region = app.game->currentRegion();
		const auto non_owned = region.allNonOwnedResources();
		const double money = app.game->money;
		const double greed = region.greed;
		previousInventory.clear();

		for (const auto &[name, amount]: app.game->inventory) {
			previousInventory.insert(name);
			auto &row = *sellRows.emplace(name, sellModel->append()).first->second;
			row[columns.resource] = name;
			row[columns.amount] = niceDouble(amount);
			row[columns.price] = niceDouble(Stonks::sellPrice(app.game->resources.at(name).basePrice,
				non_owned.count(name)? non_owned.at(name) : 0, money, greed));
		}
	}

	void MarketTab::resetBuy() {
		buyModel->clear();
		buyRows.clear();

		if (!app.game)
			return;

		auto lock = app.lockGame();
		Region &region = app.game->currentRegion();
		auto non_owned = region.allNonOwnedResources();
		double money = app.game->money;
		previousNonOwned.clear();

		for (const auto &[name, amount]: non_owned) {
			previousNonOwned.insert(name);
			auto &row = *buyRows.emplace(name, buyModel->append()).first->second;
			row[columns.resource] = name;
			row[columns.amount] = niceDouble(amount);
			row[columns.price] = niceDouble(Stonks::buyPrice(app.game->resources.at(name).basePrice, amount, money));
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

		if (!compare(previousInventory, app.game->inventory))
			resetSell();
		else
			for (const auto &[name, amount]: app.game->inventory)
				if (sellRows.count(name) != 0) {
					auto &row = *sellRows.at(name);
					row[columns.amount] = niceDouble(amount);
					row[columns.price] = niceDouble(Stonks::sellPrice(app.game->resources.at(name).basePrice,
						non_owned.count(name)? non_owned.at(name) : 0, money, greed));
				}
	}

	void MarketTab::updateBuy() {
		auto lock = app.lockGame();
		if (!app.game)
			return;

		Region &region = app.game->currentRegion();
		auto non_owned = region.allNonOwnedResources();
		double money = app.game->money;

		if (!compare(previousNonOwned, non_owned))
			resetBuy();
		else
			for (const auto &[name, amount]: non_owned)
				if (buyRows.count(name) != 0) {
					auto &row = *buyRows.at(name);
					row[columns.amount] = niceDouble(amount);
					row[columns.price] = niceDouble(Stonks::buyPrice(app.game->resources.at(name).basePrice, amount,
					                                                 money));
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
