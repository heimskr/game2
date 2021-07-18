#include <iostream>

#include "App.h"
#include "UI.h"
#include "Util.h"
#include "tab/InventoryTab.h"
#include "ui/EntryDialog.h"
#include "ui/NumericEntry.h"

namespace Game2 {
	InventoryTab::InventoryTab(App &app_): app(app_) {
		scrolled.set_child(grid);
		setMargins(grid, 5);
		grid.set_row_spacing(5);
		grid.set_column_spacing(15);
	}

	void InventoryTab::reset() {
		removeChildren(grid);
		auto lock = app.lockGame();
		if (!app.game)
			return;
		size_t row = 0;
		for (const auto &[resource_name, amount]: app.game->inventory)
			insertRow(resource_name, amount, row++);
	}

	void InventoryTab::update() {
		auto lock = app.lockGame();
		if (!app.game)
			return;

		std::vector<std::string> removed;

		for (const auto &[name, label]: nameLabels)
			if (app.game->inventory.count(name) == 0)
				removed.push_back(name);

		for (const std::string &name: removed) {
			std::cout << "Removing " << name << " from inventory\n";
			grid.remove(nameLabels.at(name));
			grid.remove(amountLabels.at(name));
			grid.remove(discardButtons.at(name));
			nameLabels.erase(name);
			amountLabels.erase(name);
			discardButtons.erase(name);
		}

		unsigned row = 0;
		for (const auto &[name, amount]: app.game->inventory) {
			if (nameLabels.count(name) == 0) {
				grid.insert_row(row);
				insertRow(name, amount, row);
			} else
				amountLabels.at(name).set_label(niceDouble(amount));
			++row;
		}
	}

	void InventoryTab::insertRow(const std::string &resource_name, double amount, int row) {
		auto &button = discardButtons.emplace(resource_name, Gtk::Button()).first->second;
		button.set_tooltip_text("Discard resource");
		button.set_icon_name("list-remove-symbolic");
		button.signal_clicked().connect([this, resource_name] {
			auto *dialog = new EntryDialog<NumericEntry>("Discard Resource", *app.mainWindow, "Amount to discard:");
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
					discard(resource_name, amount);
				});
			});
			app.dialog->show();
		});
		grid.attach(button, 0, row);

		auto &name_label = nameLabels.emplace(resource_name, resource_name).first->second;
		const std::string &description = app.game->resources.at(resource_name).description;
		if (!description.empty())
			name_label.set_tooltip_text(description);
		name_label.set_halign(Gtk::Align::START);
		grid.attach(name_label, 1, row);

		auto &amount_label = amountLabels.emplace(resource_name, niceDouble(amount)).first->second;
		amount_label.set_halign(Gtk::Align::START);
		grid.attach(amount_label, 2, row);
	}

	bool InventoryTab::discard(const std::string &resource_name, double amount) {
		if (lte(amount, 0.)) {
			app.error("Invalid amount.");
			return false;
		}

		try {
			double &in_inventory = app.game->inventory.at(resource_name);
			if (ltna(in_inventory, amount)) {
				app.error("You don't have enough of that resource.");
				return false;
			}

			if ((in_inventory -= amount) < Resource::MIN_AMOUNT)
				app.game->inventory.erase(resource_name);
		} catch (std::out_of_range &) {
			app.error("Resource not in inventory.");
			return false;
		}

		return true;
	}
}
