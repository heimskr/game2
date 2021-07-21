#pragma once

#include "Toolkit.h"
#include <memory>
#include <vector>

#include "Game.h"
#include "tab/Tab.h"

namespace Game2 {
	class App;

	class AutomationTab: public Tab {
		public:
			struct Columns: public Gtk::TreeModelColumnRecord {
				Columns() {
					add(source);
					add(destination);
					add(resource);
					add(weight);
					add(iter);
				}

				Gtk::TreeModelColumn<Glib::ustring> source, destination, resource, weight;
				Gtk::TreeModelColumn<decltype(Game::automationLinks)::iterator> iter;
			};

			App &app;

			AutomationTab() = delete;
			AutomationTab(const AutomationTab &) = delete;
			AutomationTab(AutomationTab &&) = delete;
			AutomationTab(App &);

			AutomationTab & operator=(const AutomationTab &) = delete;
			AutomationTab & operator=(AutomationTab &&) = delete;

			Gtk::Widget & getWidget() override { return scrolled; }
			Glib::ustring getName() override { return "Automation"; }
			void onFocus() override;
			void onBlur() override;

			void reset();

		private:
			Gtk::ScrolledWindow scrolled;
			std::unique_ptr<Gtk::Button> addButton, removeButton;
			std::vector<std::unique_ptr<Gtk::Widget>> widgets;
			Columns columns;
			Gtk::TreeView treeView;
			Glib::RefPtr<Gtk::ListStore> treeModel;

			void addLink();
			void removeLink();
	};
}
