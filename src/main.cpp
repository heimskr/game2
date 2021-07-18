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

constexpr long UPDATE_PERIOD = 25;

int main(int argc, char *argv[]) {
	srand(time(nullptr));
	Game2::app = std::make_unique<Game2::App>(Gtk::Application::create("com.heimskr.game2"));

	Game2::app->tickThread = std::thread([&] {
		while (Game2::app->alive) {
			{
				auto lock = Game2::app->lockGame();
				if (Game2::app->game) {
					Game2::app->game->tick(UPDATE_PERIOD / 1000.);
					Game2::app->updateDispatcher.emit();
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_PERIOD));
		}
	});

	const int status = Game2::app->run(argc, argv);
	Game2::app->alive = false;
	Game2::app->tickThread.join();
	return status;
}
