#pragma once

#include <gtkmm-4.0/gtkmm.h>

class App;

class TravelTab {
	public:
		static constexpr int ROWS = 5, COLUMNS = 5;
		static constexpr int BUTTON_COUNT = ROWS * COLUMNS;

		App &app;
		Gtk::Grid grid;
		Gtk::Button buttons[BUTTON_COUNT];

		TravelTab() = delete;
		TravelTab(const TravelTab &) = delete;
		TravelTab(TravelTab &&) = delete;
		TravelTab(App &);

		TravelTab & operator=(const TravelTab &) = delete;
		TravelTab & operator=(TravelTab &&) = delete;

		void update();

	private:
		Region::Position getPosition(Region &, int row, int column);
};
