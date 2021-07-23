#pragma once

#include "Toolkit.h"
#include <memory>
#include <string>
#include <unordered_map>

#include "tab/Tab.h"

namespace Game2 {
	class App;

	class InventoryTab: public Tab {
		public:
			struct Columns: public Gtk::TreeModelColumnRecord {
				Columns() {
					add(resource);
					add(amount);
					add(description);
				}

				Gtk::TreeModelColumn<Glib::ustring> resource, description;
				Gtk::TreeModelColumn<double> amount;
			};

			App &app;

			InventoryTab() = delete;
			InventoryTab(const InventoryTab &) = delete;
			InventoryTab(InventoryTab &&) = delete;
			InventoryTab(App &);

			InventoryTab & operator=(const InventoryTab &) = delete;
			InventoryTab & operator=(InventoryTab &&) = delete;

			Gtk::Widget & getWidget() override { return scrolled; }
			Glib::ustring getName() override { return "Inventory"; }
			void onFocus() override;
			void onBlur() override;

			void update();

		private:
			Gtk::ScrolledWindow scrolled;
			Gtk::TreeView treeView;
			Glib::RefPtr<Gtk::ListStore> treeModel;
			std::unique_ptr<Gtk::Button> discardButton;
			Columns columns;
			std::unordered_map<std::string, Gtk::TreeModel::iterator> rows;

			void discardClicked();
			bool discard(const std::string &resource_name, double amount);
	};
}
