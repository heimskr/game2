#pragma once

#include "Toolkit.h"
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
			struct Columns: public Gtk::TreeModelColumnRecord {
				Columns() {
					add(resource);
					add(amount);
					add(price);
				}

				Gtk::TreeModelColumn<Glib::ustring> resource, amount, price;
			};

			App &app;

			MarketTab() = delete;
			MarketTab(const MarketTab &) = delete;
			MarketTab(MarketTab &&) = delete;
			MarketTab(App &);

			MarketTab & operator=(const MarketTab &) = delete;
			MarketTab & operator=(MarketTab &&) = delete;

			Gtk::Widget & getWidget() override { return box; }
			Glib::ustring getName() override { return "Market"; }
			void onFocus() override;
			void onBlur() override;

			void reset();
			void updateMoney();
			void updateSell();
			void updateBuy();
			void update();

		private:
			Gtk::Box box {Gtk::Orientation::VERTICAL}, tableBox {Gtk::Orientation::HORIZONTAL};
			Gtk::ScrolledWindow scrolled;
			Gtk::Grid topGrid;
			Gtk::TreeView sellView, buyView;
			Glib::RefPtr<Gtk::ListStore> sellModel, buyModel;
			Gtk::Label regionMoneyLabel {"Region money"}, yourMoneyLabel {"Your money"}, regionMoney, yourMoney,
			           errorLabel {"Region has no market."};
			std::unique_ptr<Gtk::Button> sellButton, buyButton;
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
