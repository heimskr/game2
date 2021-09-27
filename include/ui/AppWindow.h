#pragma once

#include "Toolkit.h"
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace Game2 {
	class Game;
	class Tab;
	class RegionTab;
	class TravelTab;
	class InventoryTab;
	class ExtractionsTab;
	class ConversionTab;
	class MarketTab;
	class AutomationTab;
	class CraftingTab;
	class HelpTab;
	class ResourcesTab;

	class AppWindow: public Gtk::ApplicationWindow {
		public:
			std::shared_ptr<Game> game;
			std::unique_ptr<Gtk::Dialog> dialog;
			Glib::Dispatcher updateDispatcher, moneyDispatcher;

			bool alive = true;

			Gtk::HeaderBar *header = nullptr;
			std::vector<Gtk::Button> travelButtons;
			std::vector<std::unique_ptr<Gtk::Widget>> areaWidgets;
			std::vector<Gtk::Widget *> titleWidgets;
			std::recursive_mutex gameMutex;
			std::thread tickThread;
			std::shared_ptr<RegionTab> regionTab;
			std::shared_ptr<TravelTab> travelTab;
			std::shared_ptr<InventoryTab> inventoryTab;
			std::shared_ptr<ExtractionsTab> extractionsTab;
			std::shared_ptr<ConversionTab> conversionTab;
			std::shared_ptr<MarketTab> marketTab;
			std::shared_ptr<AutomationTab> automationTab;
			std::shared_ptr<CraftingTab> craftingTab;
			std::shared_ptr<HelpTab> helpTab;
			std::shared_ptr<ResourcesTab> resourcesTab;
			int64_t updatePeriod = 25;

			AppWindow(BaseObjectType *, const Glib::RefPtr<Gtk::Builder> &);

			~AppWindow();

			static AppWindow * create();

			std::unique_lock<std::recursive_mutex> lockGame() { return std::unique_lock(gameMutex); }
			void init();
			void update();
			void resetTitle();
			void delay(std::function<void()>);
			void alert(const Glib::ustring &message, Gtk::MessageType = Gtk::MessageType::INFO, bool modal = true,
					bool use_markup = false);
			void error(const Glib::ustring &message, bool modal = true, bool use_markup = false);
			void onTravel();
			void hackWindow();

		private:
			Glib::RefPtr<Gtk::Builder> builder;
			Glib::RefPtr<Gtk::CssProvider> cssProvider;
			Gtk::Notebook notebook;

			std::shared_ptr<Tab> activeTab;
			std::vector<std::shared_ptr<Tab>> tabs;
			sigc::connection notebookConnection;

			void connectSave();
			void saveGame();
			void choosePath(std::function<void()> after);
			void addTab(std::shared_ptr<Tab>);
	};
}
