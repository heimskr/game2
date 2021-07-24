#pragma once

#include "Toolkit.h"

#include "Position.h"
#include "tab/Tab.h"

namespace Game2 {
	class AppWindow;
	class Region;

	class TravelTab: public Tab {
		public:
			TravelTab() = delete;
			TravelTab(const TravelTab &) = delete;
			TravelTab(TravelTab &&) = delete;
			TravelTab(AppWindow &);

			TravelTab & operator=(const TravelTab &) = delete;
			TravelTab & operator=(TravelTab &&) = delete;

			Gtk::Widget & getWidget() override { return grid; }
			Glib::ustring getName() override { return "Travel"; }
			void reset();

		private:
			static constexpr int ROWS = 5, COLUMNS = 5;
			static constexpr int BUTTON_COUNT = ROWS * COLUMNS;
			AppWindow &appWindow;
			Gtk::Grid grid;
			Gtk::Button buttons[BUTTON_COUNT];
			Position getPosition(Region &, int row, int column);
	};
}
