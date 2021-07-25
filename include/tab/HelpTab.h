#pragma once

#include "Toolkit.h"
#include <memory>
#include <string>
#include <unordered_map>

#include "tab/Tab.h"

namespace Game2 {
	class AppWindow;

	class HelpTab: public Tab {
		public:
			AppWindow &appWindow;

			HelpTab() = delete;
			HelpTab(const HelpTab &) = delete;
			HelpTab(HelpTab &&) = delete;
			HelpTab(AppWindow &);

			HelpTab & operator=(const HelpTab &) = delete;
			HelpTab & operator=(HelpTab &&) = delete;

			Gtk::Widget & getWidget() override { return scrolled; }
			Glib::ustring getName() override { return "Help"; }

		private:
			Gtk::ScrolledWindow scrolled;
			Gtk::Label textWidget;
	};
}
