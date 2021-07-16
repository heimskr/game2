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
		Glib::Dispatcher dispatcher;
		std::shared_ptr<Game> game;
		std::unique_ptr<Gtk::Dialog> dialog;
		std::vector<Gtk::Button> travelButtons;
		std::recursive_mutex gameMutex;
		std::thread tickThread;
		bool alive = true;

		App(Glib::RefPtr<Gtk::Application> gtk_app): gtkApp(gtk_app), builder(Gtk::Builder::create()) {}

		void quit();
		void updateRegion();
		void updateTravel();

		std::unique_lock<std::recursive_mutex> lockGame() { return std::unique_lock(gameMutex); }

	private:
		static constexpr int ROWS = 5, COLUMNS = 5;
		Region::Position getPosition(Region &, int row, int column);
};

extern std::unique_ptr<App> app;

template <typename T>
T * getWidget(const char *name) {
	T *widget;
	app->builder->get_widget<T>(name, widget);
	return widget;
}

template <typename T>
void setMargins(T &widget, int margin) {
	widget.set_margin_top(margin);
	widget.set_margin_right(margin);
	widget.set_margin_bottom(margin);
	widget.set_margin_left(margin);
}
