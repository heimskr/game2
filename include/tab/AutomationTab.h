#pragma once

#include "Toolkit.h"
#include <memory>
#include <vector>

#include "tab/Tab.h"

namespace Game2 {
	class App;

	class AutomationTab: public Tab {
		public:
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
			Gtk::Grid grid;
			Gtk::Label sourceLabel {"Source"}, destinationLabel {"Destination"}, resourceLabel {"Resource"},
			           weightLabel {"Weight"};
			std::unique_ptr<Gtk::Button> addButton;
			std::vector<std::unique_ptr<Gtk::Widget>> widgets;
	};
}
