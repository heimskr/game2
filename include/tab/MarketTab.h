#pragma once

#include "Toolkit.h"
#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "tab/Tab.h"

namespace Game2 {
	class AppWindow;

	class MarketTab: public Tab {
		public:
			struct Columns: public Gtk::TreeModelColumnRecord {
				Columns() {
					add(resource);
					add(amount);
					add(price);
				}

				Gtk::TreeModelColumn<Glib::ustring> resource;
				Gtk::TreeModelColumn<double> amount, price;
			};

			AppWindow &appWindow;

			MarketTab() = delete;
			MarketTab(const MarketTab &) = delete;
			MarketTab(MarketTab &&) = delete;
			MarketTab(AppWindow &);

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
			Gtk::Box box {Gtk::Orientation::VERTICAL}, tableBox {Gtk::Orientation::HORIZONTAL};
			Gtk::ScrolledWindow sellScrolled, buyScrolled;
			Gtk::Grid topGrid;
			Gtk::TreeView sellView, buyView;
			Glib::RefPtr<Gtk::ListStore> sellModel, buyModel;
			Gtk::Label regionMoneyLabel {"Region money"}, yourMoneyLabel {"Your money"}, regionMoney, yourMoney,
			           errorLabel {"Region has no market."};
			Columns columns;
			std::unordered_map<std::string, Gtk::TreeModel::iterator> sellRows, buyRows;
			std::unordered_set<std::string> previousInventory, previousNonOwned;

			void resetSell();
			void resetBuy();
			void sell(const std::string &resource_name);
			void buy(const std::string &resource_name);
			void sellRow();
			void buyRow();
	};
}
