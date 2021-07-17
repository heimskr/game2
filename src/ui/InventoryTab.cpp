#include <iostream>

#include "App.h"
#include "UI.h"
#include "ui/InventoryTab.h"

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
		std::cout << "Removing " << name << "\n";
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
			amountLabels.at(name).set_label(std::to_string(amount));
		++row;
	}
}

void InventoryTab::insertRow(const std::string &resource_name, double amount, int row) {
	auto &button = discardButtons.emplace(resource_name, Gtk::Button()).first->second;
	button.set_tooltip_text("Discard resource");
	button.set_icon_name("list-remove-symbolic");
	grid.attach(button, 0, row);

	auto &name_label = nameLabels.emplace(resource_name, resource_name).first->second;
	const std::string &description = app.game->resources.at(resource_name).description;
	if (!description.empty())
		name_label.set_tooltip_text(description);
	name_label.set_halign(Gtk::Align::START);
	grid.attach(name_label, 1, row);

	auto &amount_label = amountLabels.emplace(resource_name, std::to_string(amount)).first->second;
	amount_label.set_halign(Gtk::Align::START);
	grid.attach(amount_label, 2, row);
}
