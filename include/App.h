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

		App(Glib::RefPtr<Gtk::Application> gtk_app): gtkApp(gtk_app), builder(Gtk::Builder::create()) {}

		void quit();
		void updateRegion();
};

extern std::unique_ptr<App> app;

template <typename T>
T * getWidget(const char *name) {
	T *widget;
	app->builder->get_widget<T>(name, widget);
	return widget;
}
