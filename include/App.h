#pragma once

#include <gtkmm-3.0/gtkmm.h>
#include <memory>

#include "Game.h"

class App {
	public:
		Glib::RefPtr<Gtk::Application> gtkApp;
		Glib::RefPtr<Gtk::Builder> builder;
		Glib::Dispatcher dispatcher;
		Game game;
		std::unique_ptr<Gtk::Dialog> dialog;
		std::vector<Gtk::Button> travelButtons;

		App(Glib::RefPtr<Gtk::Application> gtk_app): gtkApp(gtk_app), builder(Gtk::Builder::create()) {}

		void quit();
		void updateRegion();
		void updateTravel();
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
