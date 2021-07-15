#include <gtkmm-3.0/gtkmm.h>
#include <iostream>

int main(int argc, char *argv[]) {
	auto app = Gtk::Application::create(argc, argv, "com.heimskr.game2");

	app->signal_activate().connect([&app] {
		auto builder = Gtk::Builder::create();
		builder->add_from_file("main.glade");

		Gtk::Window *window;
		builder->get_widget("mainwindow", window);

		app->add_window(*window);
		window->show();
	});

	app->run();
	return 0;
}