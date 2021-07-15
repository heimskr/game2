#include <functional>
#include <gtkmm-3.0/gtkmm.h>
#include <iostream>
#include <memory>

#include "Game.h"
#include "App.h"

void connect(const char *name, std::function<void()> fn) {
	Gtk::MenuItem *item;
	app->builder->get_widget(name, item);
	item->signal_activate().connect(fn);
}

int main(int argc, char *argv[]) {
	app = std::make_unique<App>(Gtk::Application::create(argc, argv, "com.heimskr.game2"));
	app->builder->add_from_file("main.glade");

	auto *window = getWidget<Gtk::Window>("mainwindow");
	auto *notebook = getWidget<Gtk::Notebook>("notebook");

	notebook->hide();

	connect("new",  [&] { notebook->show(); app->game.loadDefaults(); app->updateRegion(); });
	connect("load", [&] { notebook->show(); app->game.load();         app->updateRegion(); });
	connect("save", [&] { app->game.save(); });
	connect("quit", [&] { app->quit(); });

	return app->gtkApp->run(*window);
}
