#include <iostream>

#include "App.h"
#include "UI.h"
#include "ui/InventoryTab.h"

InventoryTab::InventoryTab(App &app_): app(app_) {
	scrolled.set_child(grid);
	setMargins(grid, 5);
	grid.set_row_spacing(5);
	grid.set_column_spacing(5);
}

void InventoryTab::reset() {
	removeChildren(grid);
	auto lock = app.lockGame();
	if (!app.game) {
		std::cout << "no.\n";
		return;
	}
	std::cout << "yes.\n";
	size_t row = 0;
	for (const auto &[resource_name, amount]: app.game->inventory) {
		auto *button = new Gtk::Button;
		button->set_tooltip_text("Discard resource");

		button->set_icon_name("list-remove-symbolic");
		widgets.emplace_back(button);
		grid.attach(*button, 0, row);

		auto *label = new Gtk::Label(resource_name);
		const std::string &description = app.game->resources.at(resource_name).description;
		if (!description.empty())
			label->set_tooltip_text(description);
		widgets.emplace_back(label);
		grid.attach(*label, 1, row);

		label = new Gtk::Label(std::to_string(amount));
		widgets.emplace_back(label);
		grid.attach(*label, 2, row);

		++row;
	}
}

void InventoryTab::update() {
	
}