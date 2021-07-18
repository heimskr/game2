#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <memory>
#include <mutex>
#include <thread>

#include "Game.h"
#include "tab/RegionTab.h"
#include "tab/TravelTab.h"
#include "tab/InventoryTab.h"
#include "tab/ExtractionsTab.h"
#include "tab/ConversionTab.h"
#include "tab/MarketTab.h"

namespace Game2 {
	class App {
		public:
			Glib::RefPtr<Gtk::Application> gtkApp;
			Glib::RefPtr<Gtk::Builder> builder;
			Glib::RefPtr<Gtk::CssProvider> cssProvider;

			std::unique_ptr<Gtk::ApplicationWindow> mainWindow;
			Gtk::HeaderBar *header;
			std::unique_ptr<Gtk::Notebook> notebook;

			Glib::Dispatcher updateDispatcher, moneyDispatcher;
			std::shared_ptr<Game> game;
			std::unique_ptr<Gtk::Dialog> dialog;
			std::recursive_mutex gameMutex;
			std::thread tickThread;
			std::shared_ptr<RegionTab> regionTab;
			std::shared_ptr<TravelTab> travelTab;
			std::shared_ptr<InventoryTab> inventoryTab;
			std::shared_ptr<ExtractionsTab> extractionsTab;
			std::shared_ptr<ConversionTab> conversionTab;
			std::shared_ptr<MarketTab> marketTab;
			bool alive = true;

			std::vector<Gtk::Button> travelButtons;
			std::vector<std::unique_ptr<Gtk::Widget>> areaWidgets;
			std::vector<Gtk::Widget *> titleWidgets;

			App(Glib::RefPtr<Gtk::Application>);

			void quit();
			void delay(std::function<void()>);
			void alert(const Glib::ustring &message, Gtk::MessageType = Gtk::MessageType::INFO, bool modal = true,
					bool use_markup = false);
			void error(const Glib::ustring &message, bool modal = true, bool use_markup = false);

			std::unique_lock<std::recursive_mutex> lockGame() { return std::unique_lock(gameMutex); }

			int run(int argc, char **argv);
			void resetTitle();
			void onTravel();
			void update();

		private:
			static constexpr int ROWS = 5, COLUMNS = 5;

			std::vector<std::shared_ptr<Tab>> tabs;
			std::shared_ptr<Tab> activeTab;

			sigc::connection notebookConnection;
			void connectSave();
			void addTab(std::shared_ptr<Tab>);
			void hackWindow();
	};

	extern std::unique_ptr<App> app;
}
