#pragma once

#include <gtkmm-4.0/gtkmm.h>

class App;

class InventoryTab {
	public:
		App &app;
		Gtk::Grid grid;

		InventoryTab() = delete;
		InventoryTab(const InventoryTab &) = delete;
		InventoryTab(InventoryTab &&) = delete;
		InventoryTab(App &);

		InventoryTab & operator=(const InventoryTab &) = delete;
		InventoryTab & operator=(InventoryTab &&) = delete;

		void update();
};
