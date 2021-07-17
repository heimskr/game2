#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <memory>
#include <mutex>
#include <thread>

#include "Game.h"
#include "ui/RegionTab.h"
#include "ui/TravelTab.h"
#include "ui/InventoryTab.h"

class App {
	public:
		Glib::RefPtr<Gtk::Application> gtkApp;
		Glib::RefPtr<Gtk::Builder> builder;
		Glib::RefPtr<Gtk::CssProvider> cssProvider;

		std::unique_ptr<Gtk::ApplicationWindow> mainWindow;
		Gtk::HeaderBar *header;
		std::unique_ptr<Gtk::Notebook> notebook;
		Gtk::Button *saveButton = nullptr;

		Glib::Dispatcher updateDialogDispatcher;
		std::shared_ptr<Game> game;
		std::unique_ptr<Gtk::Dialog> dialog;
		std::recursive_mutex gameMutex;
		std::thread tickThread;
		std::unique_ptr<RegionTab> regionTab;
		std::unique_ptr<TravelTab> travelTab;
		std::unique_ptr<InventoryTab> inventoryTab;
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

	private:
		static constexpr int ROWS = 5, COLUMNS = 5;
		void updateDialog();
};

extern std::unique_ptr<App> app;
