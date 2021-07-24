#include <functional>
#include "Toolkit.h"
#include <iostream>
#include <memory>

#include "NameGen.h"
#include "Game.h"
#include "App.h"
#include "UI.h"
#include "ui/EntryDialog.h"
#include "ui/UpdatingDialog.h"

int main(int argc, char *argv[]) {
	srand(time(nullptr));
	Game2::global_app = Game2::App::create();
	return Game2::global_app->run(argc, argv);
}
