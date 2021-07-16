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

	connect("new",  [&] { notebook->show(); app->game = Game::loadDefaults(); app->updateRegion(); app->updateTravel(); });
	connect("load", [&] { notebook->show(); app->game = Game::load();         app->updateRegion(); app->updateTravel(); });
	connect("save", [&] { app->game->save(); });
	connect("quit", [&] { app->quit(); });

	notebook->hide();

	auto *rename = getWidget<Gtk::Button>("rename_region");
	rename->signal_clicked().connect([&] {
		auto *dialog = new EntryDialog("Rename Region", *window, "New region name:");
		app->dialog.reset(dialog);
		dialog->set_text(NameGen::makeRandomLanguage().makeName());
		dialog->signal_submit().connect([&](Glib::ustring str) {
			app->game->updateName(app->game->currentRegion(), str);
			app->updateRegion();
			app->updateTravel();
		});
		dialog->show();
	});

	getWidget<Gtk::Button>("delete_region")->signal_clicked().connect([&] {
		if (1 < app->game->regions.size()) {
			auto *dialog = new Gtk::MessageDialog(*window, "Are you sure you want to delete " +
				app->game->currentRegion().name + "?", false, Gtk::MessageType::MESSAGE_QUESTION,
				Gtk::ButtonsType::BUTTONS_OK_CANCEL, true);
			app->dialog.reset(dialog);
			app->dialog->signal_response().connect([&](int response) {
				if (response == Gtk::ResponseType::RESPONSE_OK) {
					app->game->erase(app->game->currentRegion());
					app->updateRegion();
					app->updateTravel();
				}
				app->dialog->hide();
			});
			dialog->show();
		} else {
			app->dialog.reset(new Gtk::MessageDialog(*window, "Can't delete region: no other region exists", false,
				Gtk::MessageType::MESSAGE_ERROR, Gtk::ButtonsType::BUTTONS_CLOSE, true));
			app->dialog->signal_response().connect([&](int) { app->dialog->hide(); });
			app->dialog->show();
		}
	});

	return app->gtkApp->run(*window);
}
