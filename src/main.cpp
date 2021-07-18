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

int main(int argc, char *argv[]) {
	app = std::make_unique<App>(Gtk::Application::create("com.heimskr.game2"));
	srand(time(nullptr));

	app->tickThread = std::thread([&] {
		while (app->alive) {
			{
				auto lock = app->lockGame();
				if (app->game) {
					app->game->tick(0.01);
					app->updateDispatcher.emit();
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
