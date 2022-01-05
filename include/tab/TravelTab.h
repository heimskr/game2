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
			void onFocus() override;
			void onBlur() override;
			void reset();

		private:
			int rows = 7, columns = 7;
			int buttonCount() const { return rows * columns; }

			std::unique_ptr<Gtk::Button> zoomOutButton, zoomInButton;
			AppWindow &appWindow;
			Gtk::Grid grid;
			std::unique_ptr<Gtk::Button[]> buttons;
			size_t buttonsLength = 0;
			Position getPosition(Region &, int row, int column);
	};
}
