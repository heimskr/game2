#include "Game.h"
#include "UI.h"
#include "ui/AppWindow.h"
#include "ui/RecipesDialog.h"

namespace Game2 {
	RecipesDialog::RecipesDialog(const Glib::ustring &title, Gtk::Window &parent, AppWindow &app_window, bool modal):
	Dialog(title, parent, modal), appWindow(app_window) {
		set_default_size(300, 300);
		scrolled.set_vexpand(true);
		scrolled.set_child(recipesBox);
		setMargins(recipesBox, 5);
		recipesBox.set_spacing(5);
		set_child(scrolled);
		auto lock = appWindow.lockGame();
		for (const auto &recipe: appWindow.game->recipes.crafting) {
			auto &label = labels.emplace_back(recipe.output +
				(recipe.amount == 1? "" : " x " + niceDouble(recipe.amount)), Gtk::Align::START);
			std::vector<std::string> pieces;
			for (const auto &[name, amount]: recipe.inputs)
				pieces.push_back(name + " x " + niceDouble(amount));
			label.set_tooltip_text(join(pieces, "\n"));
			auto &gesture = gestures.emplace_back(Gtk::GestureClick::create());
			gesture->signal_pressed().connect([this, &recipe](int, double, double) {
				signal_submit_.emit(recipe);
				hide();
			});
			label.add_controller(gesture);
			recipesBox.append(label);
		}
	}
}
