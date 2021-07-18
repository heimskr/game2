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

namespace Game2 {
	class App {
		public:
			Glib::RefPtr<Gtk::Application> gtkApp;
			Glib::RefPtr<Gtk::Builder> builder;
			Glib::RefPtr<Gtk::CssProvider> cssProvider;

			std::unique_ptr<Gtk::ApplicationWindow> mainWindow;
			Gtk::HeaderBar *header;
			std::unique_ptr<Gtk::Notebook> notebook;
			Gtk::Button *saveButton = nullptr;

			Glib::Dispatcher updateDispatcher;
			std::shared_ptr<Game> game;
			std::unique_ptr<Gtk::Dialog> dialog;
			std::recursive_mutex gameMutex;
			std::thread tickThread;
			std::unique_ptr<RegionTab> regionTab;
			std::unique_ptr<TravelTab> travelTab;
			std::unique_ptr<InventoryTab> inventoryTab;
			std::unique_ptr<ExtractionsTab> extractionsTab;
			std::unique_ptr<ConversionTab> conversionTab;
			bool alive = true;

			std::vector<Gtk::Button> travelButtons;
			std::vector<std::unique_ptr<Gtk::Widget>> areaWidgets;

			App(Glib::RefPtr<Gtk::Application>);

			void quit();
			void delay(std::function<void()>);
			void alert(const Glib::ustring &message, Gtk::MessageType = Gtk::MessageType::INFO, bool modal = true,
					bool use_markup = false);
			void error(const Glib::ustring &message, bool modal = true, bool use_markup = false);

			std::unique_lock<std::recursive_mutex> lockGame() { return std::unique_lock(gameMutex); }

			int run(int argc, char **argv);
			void onTravel();
			void update();

		private:
			static constexpr int ROWS = 5, COLUMNS = 5;

			void addTab(Tab &);
			void hackWindow();
	};

	extern std::unique_ptr<App> app;
}
