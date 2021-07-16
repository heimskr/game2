#include <functional>
#include <gtkmm-4.0/gtkmm.h>
#include <iostream>
#include <memory>

#include "NameGen.h"
#include "Game.h"
#include "App.h"
#include "UI.h"
#include "ui/EntryDialog.h"
#include "ui/UpdatingDialog.h"

void connect(const char *name, std::function<void()> fn) {
	// Gtk::MenuItem *item;
	// app->builder->get_widget(name, item);
	// item->signal_activate().connect(fn);
}

int main(int argc, char *argv[]) {
	app = std::make_unique<App>(Gtk::Application::create("com.heimskr.game2"));

	// connect("new",  [&] {
	// 	notebook->show();
	// 	app->game = Game::loadDefaults();
	// 	app->regionTab->update();
	// 	app->updateTravel();
	// });
	// connect("load", [&] {
	// 	notebook->show();
	// 	app->game = Game::load();
	// 	app->regionTab->update();
	// 	app->updateTravel();
	// });
	// connect("save", [&] { app->game->save(); });
	// connect("quit", [&] { app->quit(); });


	// auto *rename = app->builder->get_widget<Gtk::Button>("rename_region");
	// rename->signal_clicked().connect([&] {
	// 	auto dialock = app->lockDialog();
	// 	auto *dialog = new EntryDialog("Rename Region", *app->mainWindow, "New region name:");
	// 	app->dialog.reset(dialog);
	// 	dialog->set_text(NameGen::makeRandomLanguage().makeName());
	// 	dialog->signal_submit().connect([&](Glib::ustring str) {
	// 		auto lock = app->lockGame();
	// 		app->game->updateName(app->game->currentRegion(), str);
	// 		app->regionTab->update();
	// 		app->updateTravel();
	// 	});
	// 	app->dialog->show();
	// });

	// app->builder->get_widget<Gtk::Button>("delete_region")->signal_clicked().connect([&] {
	// 	auto lock = app->lockGame();
	// 	auto dialock = app->lockDialog();
	// 	if (1 < app->game->regions.size()) {
	// 		auto *dialog = new Gtk::MessageDialog(*app->mainWindow, "Are you sure you want to delete " +
	// 			app->game->currentRegion().name + "?", false, Gtk::MessageType::QUESTION,
	// 			Gtk::ButtonsType::OK_CANCEL, true);
	// 		app->dialog.reset(dialog);
	// 		app->dialog->signal_response().connect([&](int response) {
	// 			auto lock = app->lockGame();
	// 			if (response == Gtk::ResponseType::OK) {
	// 				app->game->erase(app->game->currentRegion());
	// 				app->regionTab->update();
	// 				app->updateTravel();
	// 			}
	// 			app->dialog->hide();
	// 		});
	// 		app->dialog->show();
	// 	} else {
	// 		app->dialog.reset(new Gtk::MessageDialog(*app->mainWindow, "Can't delete region: no other region exists", false,
	// 			Gtk::MessageType::ERROR, Gtk::ButtonsType::CLOSE, true));
	// 		app->dialog->signal_response().connect([&](int) { app->dialog->hide(); });
	// 		app->dialog->show();
	// 	}
	// });

	app->tickThread = std::thread([&] {
		while (app->alive) {
			{
				auto lock = app->lockGame();
				if (app->game) {
					app->game->tick(0.01);
					app->updateDialogDispatcher.emit();
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});

	const int status = app->run(argc, argv);
	app->alive = false;
	app->tickThread.join();
	return status;
}
