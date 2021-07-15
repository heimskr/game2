#include "App.h"

std::unique_ptr<App> app;

void App::quit() {
	gtkApp->quit();
}

void App::updateRegion() {
	auto *label = getWidget<Gtk::Label>("region_name");
	label->set_text(game.currentRegion().name);
}
