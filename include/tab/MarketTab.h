#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>
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
			void updateSell();
			void updateBuy();
			void update();

		private:
			Gtk::Box box {Gtk::Orientation::VERTICAL}, gridBox {Gtk::Orientation::HORIZONTAL};
			Gtk::ScrolledWindow scrolled;
			Gtk::Grid topGrid, sellGrid, buyGrid;
			Gtk::Label regionMoneyLabel {"Region money"}, yourMoneyLabel {"Your money"}, regionMoney, yourMoney;
			std::vector<std::unique_ptr<Gtk::Widget>> sellWidgets, buyWidgets;
			std::unordered_map<std::string, Gtk::Label> sellAmountLabels, sellPriceLabels;
			std::unordered_map<std::string, Gtk::Label> buyAmountLabels,  buyPriceLabels;
			std::unordered_set<std::string> previousInventory, previousNonOwned;

			void addSellHeader();
			void addBuyHeader();
			void resetSell();
			void resetBuy();
			void sell(const std::string &resource_name);
			void buy(const std::string &resource_name);
	};
}
