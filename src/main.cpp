#include <gtkmm-3.0/gtkmm.h>
#include <gtk-3.0/gtk/gtk.h>
#include <iostream>

int main(int argc, char *argv[]) {
	auto app = Gtk::Application::create(argc, argv, "com.heimskr.game2");
	auto builder = Gtk::Builder::create();
	builder->add_from_file("main.glade");

	Gtk::Window *window;
	builder->get_widget("mainwindow", window);

	Gtk::MenuItem *quit_item;
	builder->get_widget("quit", quit_item);
	quit_item->signal_activate().connect([&]() {
		app->quit();
	});

	return app->run(*window);
}
