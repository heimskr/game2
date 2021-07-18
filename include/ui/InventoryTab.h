#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <map>
#include <string>

#include "ui/Tab.h"

namespace Game2 {
	class App;

	class InventoryTab: public Tab {
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

			Gtk::Widget & getWidget() override { return scrolled; }
			Glib::ustring getName() override { return "Inventory"; }
			void reset();
			void update();

		private:
			// std::vector<std::unique_ptr<Gtk::Widget>> widgets;
			std::map<std::string, Gtk::Button> discardButtons;
			std::map<std::string, Gtk::Label> nameLabels, amountLabels;
			void insertRow(const std::string &resource_name, double amount, int row);
			bool discard(const std::string &resource_name, double amount);
	};
}
