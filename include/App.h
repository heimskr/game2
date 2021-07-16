#pragma once

#include <gtkmm-3.0/gtkmm.h>
#include <memory>
#include <mutex>
#include <thread>

#include "Game.h"

class App {
	public:
		Glib::RefPtr<Gtk::Application> gtkApp;
		Glib::RefPtr<Gtk::Builder> builder;
		Gtk::Window *mainWindow;
		Glib::Dispatcher dispatcher;
		std::shared_ptr<Game> game;
		std::unique_ptr<Gtk::Dialog> dialog;
		std::recursive_mutex gameMutex;
		std::thread tickThread;
		bool alive = true;

		std::vector<Gtk::Button> travelButtons;

		std::shared_ptr<Region> lastRegion;
		std::vector<std::unique_ptr<Gtk::Widget>> areaWidgets;

		App(Glib::RefPtr<Gtk::Application> gtk_app): gtkApp(gtk_app), builder(Gtk::Builder::create()) {}

		void quit();
		void updateRegion();
		void updateTravel();

		std::unique_lock<std::recursive_mutex> lockGame() { return std::unique_lock(gameMutex); }
		int run() { return gtkApp->run(*mainWindow); }

	private:
		static constexpr int ROWS = 5, COLUMNS = 5;
		Region::Position getPosition(Region &, int row, int column);
};

extern std::unique_ptr<App> app;
