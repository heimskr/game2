#include "App.h"
#include "UI.h"
#include "tab/CraftingTab.h"
#include "ui/EntryDialog.h"
#include "ui/InventoryDialog.h"
#include "ui/NumericEntry.h"
#include "ui/RecipesDialog.h"

namespace Game2 {
	CraftingTab::CraftingTab(App &app_): app(app_) {
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

		reset(false);
	}

	void CraftingTab::reset(bool compute_crafting) {
		inputModel->clear();
		outputModel->clear();
		inputRows.clear();
		outputRows.clear();

		if (!app.game)
			return;

		auto lock = app.lockGame();

		if (compute_crafting)
			computeCraftingOutput();

		for (const auto &[name, amount]: app.game->craftingInventory)
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
		auto lock = app.lockGame();
		computeCraftingOutput();

		auto input_children = inputModel->children();
		for (auto iter = input_children.begin(); iter != input_children.end();) {
			const Glib::ustring resource_name = (*iter)[columns.resource];
			if (app.game->craftingInventory.count(resource_name) == 0) {
				inputRows.erase(resource_name);
				inputModel->erase(iter++);
			} else
				++iter;
		}

		for (const auto &[name, amount]: app.game->craftingInventory)
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
		auto lock = app.lockGame();
		for (const auto &[name, amount]: recipe->inputs)
			app.game->craftingInventory[name] -= amount;
		app.game->inventory[recipe->output] += recipe->amount;
		shrink(app.game->craftingInventory);
		update();
	}
	
	void CraftingTab::add() {
		auto *dialog = new InventoryDialog("Resource Selector", *app.mainWindow);
		app.dialog.reset(dialog);
		dialog->signal_submit().connect([this](const Glib::ustring &name) {
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

	void CraftingTab::remove() {
		if (auto iter = inputView.get_selection()->get_selected()) {
			auto lock = app.lockGame();
			auto name = iter->get_value(columns.resource);
			auto amount = iter->get_value(columns.amount);
			app.game->inventory[name] += amount;
			app.game->craftingInventory.erase(name);
			update();
		}
	}

	void CraftingTab::craftClicked() {
		if (auto iter = outputView.get_selection()->get_selected())
			craft(iter->get_value(columns.recipe));
	}

	void CraftingTab::computeCraftingOutput() {
		craftingOutput.clear();
		for (const CraftingRecipe &recipe: app.game->recipes.crafting)
			if (contains(app.game->craftingInventory, recipe.inputs))
				craftingOutput.insert(&recipe);
	}

	void CraftingTab::onFocus() {
		if (!app.game)
			return;

		addButton = std::make_unique<Gtk::Button>();
		addButton->set_icon_name("list-add-symbolic");
		addButton->signal_clicked().connect(sigc::mem_fun(*this, &CraftingTab::add));
		app.header->pack_start(*addButton);
		app.titleWidgets.push_back(addButton.get());

		removeButton = std::make_unique<Gtk::Button>();
		removeButton->set_icon_name("list-remove-symbolic");
		removeButton->signal_clicked().connect(sigc::mem_fun(*this, &CraftingTab::remove));
		app.header->pack_start(*removeButton);
		app.titleWidgets.push_back(removeButton.get());

		craftButton = std::make_unique<Gtk::Button>();
		craftButton->set_icon_name("applications-engineering-symbolic"); // Probably confusing with other icon sets.
		craftButton->signal_clicked().connect(sigc::mem_fun(*this, &CraftingTab::craftClicked));
		app.header->pack_start(*craftButton);
		app.titleWidgets.push_back(craftButton.get());

		helpButton = std::make_unique<Gtk::Button>();
		helpButton->set_icon_name("help-browser-symbolic");
		helpButton->signal_clicked().connect(sigc::mem_fun(*this, &CraftingTab::showHelp));
		app.header->pack_start(*helpButton);
		app.titleWidgets.push_back(helpButton.get());
	}

	void CraftingTab::onBlur() {
		addButton.reset();
		removeButton.reset();
		craftButton.reset();
		helpButton.reset();
	}

	void CraftingTab::showHelp() {
		auto *dialog = new RecipesDialog("Recipes", *app.mainWindow, app);
		app.dialog.reset(dialog);
		dialog->signal_submit().connect([this](const CraftingRecipe &recipe) {
			auto lock = app.lockGame();
			if (contains(app.game->inventory, recipe.inputs)) {
				for (const auto &[name, amount]: recipe.inputs) {
					app.game->inventory[name] -= amount;
					app.game->craftingInventory[name] += amount;
					reset();
				}
			} else {
				app.delay([this] { app.error("You don't have enough resources."); });
			}
		});
		app.dialog->show();
	}
}
