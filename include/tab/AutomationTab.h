#pragma once

#include "Toolkit.h"
#include <memory>
#include <vector>

#include "Game.h"
#include "tab/Tab.h"

namespace Game2 {
	class AppWindow;

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

			AppWindow &appWindow;

			AutomationTab() = delete;
			AutomationTab(const AutomationTab &) = delete;
			AutomationTab(AutomationTab &&) = delete;
			AutomationTab(AppWindow &);

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
			Columns columns;
			Gtk::TreeView treeView;
			Glib::RefPtr<Gtk::ListStore> treeModel;

			void rowActivated(const Gtk::TreeModel::Path &, Gtk::TreeViewColumn *);
			void addLink();
			void removeLink();
	};
}
