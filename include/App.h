#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <memory>
#include <mutex>
#include <thread>

#include "Game.h"
#include "ui/RegionTab.h"

class App {
	public:
		Glib::RefPtr<Gtk::Application> gtkApp;
		Glib::RefPtr<Gtk::Builder> builder;
		Glib::RefPtr<Gtk::CssProvider> cssProvider;

		std::unique_ptr<Gtk::ApplicationWindow> mainWindow;
		Gtk::HeaderBar *header;
		std::unique_ptr<Gtk::Notebook> notebook;

		Glib::Dispatcher updateDialogDispatcher;
		std::shared_ptr<Game> game;
		std::unique_ptr<Gtk::Dialog> dialog;
		std::recursive_mutex gameMutex, dialogMutex;
		std::thread tickThread;
		std::unique_ptr<RegionTab> regionTab;
		bool alive = true;


		std::vector<Gtk::Button> travelButtons;
		std::vector<std::unique_ptr<Gtk::Widget>> areaWidgets;

		App(Glib::RefPtr<Gtk::Application>);

		void quit();
		void delay(std::function<void()>);
		void updateTravel();

		std::unique_lock<std::recursive_mutex> lockGame() { return std::unique_lock(gameMutex); }
		std::unique_lock<std::recursive_mutex> lockDialog() { return std::unique_lock(dialogMutex); }
		int run(int argc, char **argv) {
			gtkApp->signal_activate().connect([this] {
				gtkApp->add_window(*mainWindow);
				mainWindow->show();
			});
			return gtkApp->run(argc, argv);
		}

	private:
		static constexpr int ROWS = 5, COLUMNS = 5;
		void updateDialog();
		Region::Position getPosition(Region &, int row, int column);

};

extern std::unique_ptr<App> app;
