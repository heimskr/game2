#include <functional>
#include <gtkmm-3.0/gtkmm.h>
#include <iostream>
#include <memory>

#include "NameGen.h"
#include "Game.h"
#include "App.h"
#include "ui/EntryDialog.h"

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

	connect("new",  [&] { notebook->show(); app->game.loadDefaults(); app->updateRegion(); });
	connect("load", [&] { notebook->show(); app->game.load();         app->updateRegion(); });
	connect("save", [&] { app->game.save(); });
	connect("quit", [&] { app->quit(); });

	notebook->hide();

	auto *rename = getWidget<Gtk::Button>("rename_region");
	rename->signal_clicked().connect([&] {
		auto *dialog = new EntryDialog("Rename Region", *window, "New region name:");
		app->dialog.reset(dialog);
		dialog->set_text(NameGen::makeRandomLanguage().makeName());
		dialog->signal_submit().connect([&](Glib::ustring str) {
			app->game.updateName(app->game.currentRegion(), str);
			app->updateRegion();
		});
		dialog->show();
	});

	return app->gtkApp->run(*window);
}
