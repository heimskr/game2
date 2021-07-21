#include "App.h"
#include "UI.h"
#include "tab/CraftingTab.h"
#include "ui/EntryDialog.h"
#include "ui/InventoryDialog.h"
#include "ui/NumericEntry.h"

namespace Game2 {
	CraftingTab::CraftingTab(App &app_): app(app_) {
		scrolled.set_child(gridBox);
		scrolled.set_vexpand(true);
		setMargins(gridBox, 5);
		gridBox.set_spacing(5);
		inputGrid.set_row_spacing(5);
		outputGrid.set_row_spacing(5);
		inputGrid.set_column_spacing(5);
		outputGrid.set_column_spacing(5);
		inputGrid.set_hexpand(true);
		outputGrid.set_hexpand(true);
		gridBox.append(inputGrid);
		gridBox.append(separator);
		gridBox.append(outputGrid);
		inputLabel.set_hexpand(true);
		outputLabel.set_hexpand(true);
		inputLabel.add_css_class("table-header");
		outputLabel.add_css_class("table-header");
		inputAmountLabel.add_css_class("table-header");
		outputAmountLabel.add_css_class("table-header");
		inputLabel.set_xalign(0);
		outputLabel.set_xalign(0);
		inputAmountLabel.set_xalign(0);
		outputAmountLabel.set_xalign(0);
		reset(false);
	}

	void CraftingTab::reset(bool compute_crafting) {
		removeChildren(inputGrid);
		removeChildren(outputGrid);
		widgets.clear();

		inputGrid.attach(inputLabel, 1, 0);
		inputGrid.attach(inputAmountLabel, 2, 0);
		outputGrid.attach(outputLabel, 0, 0);
		outputGrid.attach(outputAmountLabel, 1, 0);

		if (!app.game)
			return;

		if (compute_crafting)
			computeCraftingOutput();

		auto lock = app.lockGame();
		int row = 1;
		for (const auto &pair: app.game->craftingInventory) {
			const auto &name = pair.first;
			const auto &amount = pair.second;
			auto *button = new Gtk::Button;
			widgets.emplace_back(button);
			button->set_icon_name("list-remove-symbolic");
			button->signal_clicked().connect([this, &name, &amount] {
				auto lock = app.lockGame();
				app.game->inventory[name] += amount;
				app.game->craftingInventory.erase(name);
				reset();
			});
			inputGrid.attach(*button, 0, row);
			auto *label = new Gtk::Label(name, Gtk::Align::START);
			widgets.emplace_back(label);
			inputGrid.attach(*label, 1, row);
			label = new Gtk::Label(niceDouble(amount), Gtk::Align::START);
			widgets.emplace_back(label);
			inputGrid.attach(*label, 2, row);
			++row;
		}

		row = 1;
		for (const auto *recipe: craftingOutput) {
			auto *label = new Gtk::Label(recipe->output, Gtk::Align::START);
			widgets.emplace_back(label);
			outputGrid.attach(*label, 0, row);
			label = new Gtk::Label(niceDouble(recipe->amount), Gtk::Align::START);
			widgets.emplace_back(label);
			outputGrid.attach(*label, 1, row);
			auto *button = new Gtk::Button;
			widgets.emplace_back(button);
			button->set_icon_name("applications-engineering-symbolic"); // Probably confusing with other icon sets.
			button->signal_clicked().connect([this, recipe] {
				craft(recipe);
			});
			outputGrid.attach(*button, 2, row);
			++row;
		}
	}

	void CraftingTab::craft(const CraftingRecipe *recipe) {
		auto lock = app.lockGame();
		for (const auto &[name, amount]: recipe->inputs)
			app.game->craftingInventory[name] -= amount;
		app.game->inventory[recipe->output] += recipe->amount;
		shrink(app.game->craftingInventory);
		reset();
	}
	
	void CraftingTab::add() {
		auto *dialog = new InventoryDialog("Resource Selector", *app.mainWindow);
		app.dialog.reset(dialog);
		dialog->signal_submit().connect([this](Glib::ustring name) {
			if (name.empty())
				return;
			app.delay([this, name] {
				auto *dialog = new EntryDialog<NumericEntry>("Amount", *app.mainWindow, "Amount to add:");
				app.dialog.reset(dialog);
				dialog->signal_submit().connect([this, name](const Glib::ustring &amount_str) {
					double amount;
					try {
						amount = parseDouble(amount_str);
					} catch (std::invalid_argument &) {
						app.delay([this] { app.error("Invalid amount."); });
						return;
					}
					auto lock = app.lockGame();
					if (amount == 0)
						amount = app.game->inventory[name];
					if (amount <= 0 || ltna(app.game->inventory[name], amount)) {
						app.delay([this] { app.error("Invalid amount."); });
						return;
					}
					app.game->inventory[name] -= amount;
					app.game->craftingInventory[name] += amount;
					shrink(app.game->inventory);
					reset();
				});
				app.dialog->show();
			});
		});
		app.dialog->show();
	}

	void CraftingTab::computeCraftingOutput() {
		craftingOutput.clear();
		for (const CraftingRecipe &recipe: app.game->recipes.crafting)
			if (contains(app.game->craftingInventory, recipe.inputs))
				craftingOutput.push_back(&recipe);
	}

	void CraftingTab::onFocus() {
		if (!app.game)
			return;
		addButton = std::make_unique<Gtk::Button>();
		addButton->set_icon_name("list-add-symbolic");
		addButton->signal_clicked().connect(sigc::mem_fun(*this, &CraftingTab::add));
		app.header->pack_start(*addButton);
		app.titleWidgets.push_back(addButton.get());
	}

	void CraftingTab::onBlur() {
		addButton.reset();
	}
}
