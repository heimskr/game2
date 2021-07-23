#include "App.h"
#include "UI.h"
#include "Util.h"
#include "tab/InventoryTab.h"
#include "ui/EntryDialog.h"
#include "ui/NumericEntry.h"

namespace Game2 {
	InventoryTab::InventoryTab(App &app_): app(app_) {
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
		if (!app.game)
			return;

		discardButton = std::make_unique<Gtk::Button>();
		discardButton->set_icon_name("list-remove-symbolic");
		discardButton->signal_clicked().connect(sigc::mem_fun(*this, &InventoryTab::discardClicked));
		app.header->pack_start(*discardButton);
		app.titleWidgets.push_back(discardButton.get());
	}

	void InventoryTab::onBlur() {
		discardButton.reset();
	}

	void InventoryTab::update() {
		if (!app.game)
			return;

		auto lock = app.lockGame();

		auto children = treeModel->children();
		for (auto iter = children.begin(); iter != children.end();) {
			const Glib::ustring resource_name = (*iter)[columns.resource];
			if (app.game->inventory.count(resource_name) == 0) {
				rows.erase(resource_name);
				treeModel->erase(iter++);
			} else
				++iter;
		}

		for (const auto &[name, amount]: app.game->inventory)
			if (rows.count(name) == 0) {
				auto row = treeModel->append();
				(*row)[columns.resource] = name;
				(*row)[columns.amount] = amount;
				(*row)[columns.description] = app.game->resources.at(name).description;
				rows.emplace(name, row);
			} else
				(*rows.at(name))[columns.amount] = amount;
	}

	void InventoryTab::discardClicked() {
		if (auto iter = treeView.get_selection()->get_selected()) {
			const Glib::ustring resource_name = (*iter)[columns.resource];
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
		}
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

			update();
		} catch (std::out_of_range &) {
			app.error("Resource not in inventory.");
			return false;
		}

		return true;
	}
}
