#include <functional>
#include <gtkmm-3.0/gtkmm.h>
#include <gtk-3.0/gtk/gtk.h>
#include <iostream>

#include "Game.h"

void connect(Glib::RefPtr<Gtk::Builder> &builder, const char *name, std::function<void()> fn) {
	Gtk::MenuItem *item;
	builder->get_widget(name, item);
	item->signal_activate().connect(fn);
}

int main(int argc, char *argv[]) {
	auto app = Gtk::Application::create(argc, argv, "com.heimskr.game2");
	auto builder = Gtk::Builder::create();
	builder->add_from_file("main.glade");

	Game game;

	Gtk::Window *window;
	builder->get_widget("mainwindow", window);

	connect(builder, "new",  [&] { game.loadDefaults(); });
	connect(builder, "load", [&] { game.load(); });
	connect(builder, "save", [&] { game.save(); });
	connect(builder, "quit", [&] { app->quit(); });

	return app->run(*window);
}
