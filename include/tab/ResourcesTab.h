#pragma once

#include "Toolkit.h"
#include <memory>
#include <string>
#include <unordered_map>

#include "tab/Tab.h"

namespace Game2 {
	class AppWindow;

	class ResourcesTab: public Tab {
		public:
			struct Columns: public Gtk::TreeModelColumnRecord {
				Columns() {
					add(resource);
					add(basePrice);
				}

				Gtk::TreeModelColumn<Glib::ustring> resource;
				Gtk::TreeModelColumn<double> basePrice;
			};

			AppWindow &appWindow;

			ResourcesTab() = delete;
			ResourcesTab(const ResourcesTab &) = delete;
			ResourcesTab(ResourcesTab &&) = delete;
			ResourcesTab(AppWindow &);

			ResourcesTab & operator=(const ResourcesTab &) = delete;
			ResourcesTab & operator=(ResourcesTab &&) = delete;

			Gtk::Widget & getWidget() override { return scrolled; }
			Glib::ustring getName() override { return "Resources"; }

			void reset();

		private:
			Gtk::ScrolledWindow scrolled;
			Gtk::TreeView treeView;
			Glib::RefPtr<Gtk::ListStore> treeModel;
			Columns columns;
	};
}
