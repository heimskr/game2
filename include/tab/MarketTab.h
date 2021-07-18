#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <map>
#include <memory>
#include <vector>

#include "tab/Tab.h"

namespace Game2 {
	class App;

	class MarketTab: public Tab {
		public:
			App &app;

			MarketTab() = delete;
			MarketTab(const MarketTab &) = delete;
			MarketTab(MarketTab &&) = delete;
			MarketTab(App &);

			MarketTab & operator=(const MarketTab &) = delete;
			MarketTab & operator=(MarketTab &&) = delete;

			Gtk::Widget & getWidget() override { return box; }
			Glib::ustring getName() override { return "Market"; }

			void reset();
			void updateMoney();

		private:
			Gtk::Box box {Gtk::Orientation::VERTICAL}, gridBox {Gtk::Orientation::HORIZONTAL};
			Gtk::ScrolledWindow scrolled;
			Gtk::Grid topGrid, leftGrid, rightGrid;
			Gtk::Label regionMoneyLabel {"Region money"}, yourMoneyLabel {"Your money"}, regionMoney, yourMoney;
			std::vector<std::unique_ptr<Gtk::Widget>> widgets;

			void addHeader();
			void resetGrids();
			void sell(const std::string &resource_name);
			void buy(const std::string &resource_name);
	};
}
