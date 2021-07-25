#include "Game.h"
#include "UI.h"
#include "tab/CraftingTab.h"
#include "ui/AppWindow.h"
#include "ui/EntryDialog.h"
#include "ui/InventoryDialog.h"
#include "ui/NumericEntry.h"
#include "ui/RecipesDialog.h"

namespace Game2 {
	CraftingTab::CraftingTab(AppWindow &app_window): appWindow(app_window) {
		scrolled.set_child(treeBox);
		scrolled.set_vexpand(true);
		treeBox.set_homogeneous(true);
		treeBox.append(inputView);
		treeBox.append(outputView);

		inputModel = Gtk::ListStore::create(columns);
		inputView.set_model(inputModel);
		appendColumn(inputView, "Input", columns.resource);
		appendColumn(inputView, "Amount", columns.amount);

		outputModel = Gtk::ListStore::create(columns);
		outputView.set_model(outputModel);
		appendColumn(outputView, "Output", columns.resource);
		appendColumn(outputView, "Amount", columns.amount);

		for (auto *view: {&inputView, &outputView})
			for (int i = 0, columns = view->get_n_columns(); i < columns; ++i) {
				auto *column = view->get_column(i);
				column->set_expand(true);
				column->set_resizable(true);
			}

		inputView.signal_row_activated().connect([this](const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *) {
			if (path.size() == 1)
				remove();
		});

		outputView.signal_row_activated().connect([this](const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *) {
			if (path.size() == 1)
				craftRow();
		});

		reset(false);
	}

	void CraftingTab::reset(bool compute_crafting) {
		inputModel->clear();
		outputModel->clear();
		inputRows.clear();
		outputRows.clear();

		if (!appWindow.game)
			return;

		auto lock = appWindow.lockGame();

		if (compute_crafting)
			computeCraftingOutput();

		for (const auto &[name, amount]: appWindow.game->craftingInventory)
			addInput(name, amount);

		for (const auto *recipe: craftingOutput)
			addOutput(recipe);
	}

	Gtk::TreeModel::iterator CraftingTab::addInput(const std::string &name, double amount) {
		auto row = inputModel->append();
		(*row)[columns.resource] = name;
		(*row)[columns.amount] = amount;
		inputRows.emplace(name, row);
		return row;
	}

	Gtk::TreeModel::iterator CraftingTab::addOutput(const CraftingRecipe *recipe) {
		auto row = outputModel->append();
		(*row)[columns.resource] = recipe->output;
		(*row)[columns.amount] = recipe->amount;
		(*row)[columns.recipe] = recipe;
		outputRows.emplace(recipe, row);
		return row;
	}

	void CraftingTab::update() {
		auto lock = appWindow.lockGame();
		computeCraftingOutput();

		auto input_children = inputModel->children();
		for (auto iter = input_children.begin(); iter != input_children.end();) {
			const Glib::ustring resource_name = (*iter)[columns.resource];
			if (appWindow.game->craftingInventory.count(resource_name) == 0) {
				inputRows.erase(resource_name);
				inputModel->erase(iter++);
			} else
				++iter;
		}

		for (const auto &[name, amount]: appWindow.game->craftingInventory)
			if (inputRows.count(name) == 0)
				addInput(name, amount);
			else
				(*inputRows.at(name))[columns.amount] = amount;

		auto output_children = outputModel->children();
		for (auto iter = output_children.begin(); iter != output_children.end();) {
			const CraftingRecipe *recipe = (*iter)[columns.recipe];
			if (craftingOutput.count(recipe) == 0) {
				outputRows.erase(recipe);
				outputModel->erase(iter++);
			} else
				++iter;
		}

		for (const CraftingRecipe *recipe: craftingOutput)
			if (outputRows.count(recipe) == 0)
				addOutput(recipe);
	}

	void CraftingTab::craft(const CraftingRecipe *recipe) {
		auto lock = appWindow.lockGame();
		for (const auto &[name, amount]: recipe->inputs)
			appWindow.game->craftingInventory[name] -= amount;
		appWindow.game->inventory[recipe->output] += recipe->amount;
		shrink(appWindow.game->craftingInventory);
		update();
	}

	void CraftingTab::add() {
		auto *dialog = new InventoryDialog("Resource Selector", appWindow, appWindow);
		appWindow.dialog.reset(dialog);
		dialog->signal_submit().connect([this](const Glib::ustring &name) {
			if (name.empty())
				return;
			appWindow.delay([this, name] {
				auto *dialog = new EntryDialog<NumericEntry>("Amount", appWindow, "Amount to add:");
				appWindow.dialog.reset(dialog);
				dialog->signal_submit().connect([this, name](const Glib::ustring &amount_str) {
					double amount;
					try {
						amount = parseDouble(amount_str);
					} catch (std::invalid_argument &) {
						appWindow.delay([this] { appWindow.error("Invalid amount."); });
						return;
					}
					auto lock = appWindow.lockGame();
					if (amount == 0)
						amount = appWindow.game->inventory[name];
					if (amount <= 0 || ltna(appWindow.game->inventory[name], amount)) {
						appWindow.delay([this] { appWindow.error("Invalid amount."); });
						return;
					}
					appWindow.game->inventory[name] -= amount;
					appWindow.game->craftingInventory[name] += amount;
					shrink(appWindow.game->inventory);
					reset();
				});
				appWindow.dialog->show();
			});
		});
		appWindow.dialog->show();
	}

	void CraftingTab::remove() {
		if (auto iter = inputView.get_selection()->get_selected()) {
			auto lock = appWindow.lockGame();
			auto name = iter->get_value(columns.resource);
			auto amount = iter->get_value(columns.amount);
			appWindow.game->inventory[name] += amount;
			appWindow.game->craftingInventory.erase(name);
			update();
		}
	}

	void CraftingTab::craftRow() {
		if (auto iter = outputView.get_selection()->get_selected())
			craft(iter->get_value(columns.recipe));
	}

	void CraftingTab::computeCraftingOutput() {
		craftingOutput.clear();
		for (const CraftingRecipe &recipe: appWindow.game->recipes.crafting)
			if (contains(appWindow.game->craftingInventory, recipe.inputs))
				craftingOutput.insert(&recipe);
	}

	void CraftingTab::onFocus() {
		if (!appWindow.game)
			return;

		addButton = std::make_unique<Gtk::Button>();
		addButton->set_icon_name("list-add-symbolic");
		addButton->signal_clicked().connect(sigc::mem_fun(*this, &CraftingTab::add));
		appWindow.header->pack_start(*addButton);
		appWindow.titleWidgets.push_back(addButton.get());

		helpButton = std::make_unique<Gtk::Button>();
		helpButton->set_icon_name("help-browser-symbolic");
		helpButton->signal_clicked().connect(sigc::mem_fun(*this, &CraftingTab::showHelp));
		appWindow.header->pack_start(*helpButton);
		appWindow.titleWidgets.push_back(helpButton.get());
	}

	void CraftingTab::onBlur() {
		addButton.reset();
		helpButton.reset();
	}

	void CraftingTab::showHelp() {
		auto *dialog = new RecipesDialog("Recipes", appWindow, appWindow);
		appWindow.dialog.reset(dialog);
		dialog->signal_submit().connect([this](const CraftingRecipe &recipe) {
			auto lock = appWindow.lockGame();
			if (contains(appWindow.game->inventory, recipe.inputs)) {
				for (const auto &[name, amount]: recipe.inputs) {
					appWindow.game->inventory[name] -= amount;
					appWindow.game->craftingInventory[name] += amount;
					reset();
				}
			} else {
				appWindow.delay([this] { appWindow.error("You don't have enough resources."); });
			}
		});
		appWindow.dialog->show();
	}
}
