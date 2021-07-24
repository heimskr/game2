#include "Game.h"
#include "UI.h"
#include "Util.h"
#include "tab/InventoryTab.h"
#include "ui/AppWindow.h"
#include "ui/EntryDialog.h"
#include "ui/NumericEntry.h"

namespace Game2 {
	InventoryTab::InventoryTab(AppWindow &app_window): appWindow(app_window) {
		scrolled.set_child(treeView);
		scrolled.set_vexpand(true);

		treeModel = Gtk::ListStore::create(columns);
		treeView.set_model(treeModel);
		appendColumn(treeView, "Resource", columns.resource);
		appendColumn(treeView, "Amount", columns.amount);
		appendColumn(treeView, "Description", columns.description);

		for (int i = 0, columns = treeView.get_n_columns(); i < columns; ++i) {
			auto *column = treeView.get_column(i);
			column->set_expand(true);
			column->set_resizable(true);
		}
	}

	void InventoryTab::onFocus() {
		if (!appWindow.game)
			return;

		discardButton = std::make_unique<Gtk::Button>();
		discardButton->set_icon_name("list-remove-symbolic");
		discardButton->signal_clicked().connect(sigc::mem_fun(*this, &InventoryTab::discardClicked));
		appWindow.header->pack_start(*discardButton);
		appWindow.titleWidgets.push_back(discardButton.get());
	}

	void InventoryTab::onBlur() {
		discardButton.reset();
	}

	void InventoryTab::update() {
		if (!appWindow.game)
			return;

		auto lock = appWindow.lockGame();

		auto children = treeModel->children();
		for (auto iter = children.begin(); iter != children.end();) {
			const Glib::ustring resource_name = (*iter)[columns.resource];
			if (appWindow.game->inventory.count(resource_name) == 0) {
				rows.erase(resource_name);
				treeModel->erase(iter++);
			} else
				++iter;
		}

		for (const auto &[name, amount]: appWindow.game->inventory)
			if (rows.count(name) == 0) {
				auto row = treeModel->append();
				(*row)[columns.resource] = name;
				(*row)[columns.amount] = amount;
				(*row)[columns.description] = appWindow.game->resources.at(name).description;
				rows.emplace(name, row);
			} else
				(*rows.at(name))[columns.amount] = amount;
	}

	void InventoryTab::discardClicked() {
		if (auto iter = treeView.get_selection()->get_selected()) {
			const Glib::ustring resource_name = (*iter)[columns.resource];
			auto *dialog = new EntryDialog<NumericEntry>("Discard Resource", appWindow, "Amount to discard:");
			appWindow.dialog.reset(dialog);
			dialog->signal_submit().connect([this, resource_name](const Glib::ustring &response) {
				appWindow.delay([this, resource_name, response] {
					double amount;
					if (response == ".")
						amount = appWindow.game->inventory[resource_name];
					else
						try {
							amount = parseDouble(response);
						} catch (std::invalid_argument &) {
							appWindow.error("Invalid amount.");
							return;
						}
					discard(resource_name, amount);
				});
			});
			appWindow.dialog->show();
		}
	}

	bool InventoryTab::discard(const std::string &resource_name, double amount) {
		if (lte(amount, 0.)) {
			appWindow.error("Invalid amount.");
			return false;
		}

		try {
			double &in_inventory = appWindow.game->inventory.at(resource_name);
			if (ltna(in_inventory, amount)) {
				appWindow.error("You don't have enough of that resource.");
				return false;
			}

			if ((in_inventory -= amount) < Resource::MIN_AMOUNT)
				appWindow.game->inventory.erase(resource_name);

			update();
		} catch (std::out_of_range &) {
			appWindow.error("Resource not in inventory.");
			return false;
		}

		return true;
	}
}
