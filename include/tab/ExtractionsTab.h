#pragma once

#include "Toolkit.h"
#include <memory>
#include <vector>

#include "tab/Tab.h"

namespace Game2 {
	class App;

	class ExtractionsTab: public Tab {
		public:
			App &app;

			ExtractionsTab() = delete;
			ExtractionsTab(const ExtractionsTab &) = delete;
			ExtractionsTab(ExtractionsTab &&) = delete;
			ExtractionsTab(App &);

			ExtractionsTab & operator=(const ExtractionsTab &) = delete;
			ExtractionsTab & operator=(ExtractionsTab &&) = delete;

			Gtk::Widget & getWidget() override { return scrolled; }
			Glib::ustring getName() override { return "Extractions"; }
			void onFocus() override;
			void onBlur() override;

			void reset();

		private:
			bool global = false;
			Gtk::ScrolledWindow scrolled;
			Gtk::Grid grid;
			std::unique_ptr<Gtk::ToggleButton> globalButton;
			std::vector<std::unique_ptr<Gtk::Widget>> widgets;

	};
}
