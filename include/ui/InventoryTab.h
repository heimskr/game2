#pragma once

#include <gtkmm-4.0/gtkmm.h>

class App;

class InventoryTab {
	public:
		App &app;
		Gtk::ScrolledWindow scrolled;
		Gtk::Grid grid;

		InventoryTab() = delete;
		InventoryTab(const InventoryTab &) = delete;
		InventoryTab(InventoryTab &&) = delete;
		InventoryTab(App &);

		InventoryTab & operator=(const InventoryTab &) = delete;
		InventoryTab & operator=(InventoryTab &&) = delete;

		void reset();
		void update();

	private:
		// std::vector<std::unique_ptr<Gtk::Widget>> widgets;
		std::map<std::string, Gtk::Button> discardButtons;
		std::map<std::string, Gtk::Label> nameLabels, amountLabels;
		void insertRow(const std::string &resource_name, double amount, int row);
};
