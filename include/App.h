#pragma once

#include <gtkmm-3.0/gtkmm.h>

#include "Game.h"

class App {
	public:
		Glib::RefPtr<Gtk::Application> gtkApp;
		Glib::RefPtr<Gtk::Builder> builder;
		Glib::Dispatcher dispatcher;
		Game game;

		App(Glib::RefPtr<Gtk::Application> gtk_app): gtkApp(gtk_app), builder(Gtk::Builder::create()) {}

		void quit();
};
