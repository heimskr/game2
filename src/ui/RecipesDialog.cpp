#include "App.h"
#include "UI.h"
#include "ui/RecipesDialog.h"

namespace Game2 {
	RecipesDialog::RecipesDialog(const Glib::ustring &title, Gtk::Window &parent, App &app_, bool modal):
	Dialog(title, parent, modal), app(app_) {
		set_default_size(300, 300);
		scrolled.set_vexpand(true);
		scrolled.set_child(recipesBox);
		setMargins(recipesBox, 5);
		recipesBox.set_spacing(5);
		set_child(scrolled);
		auto lock = app.lockGame();
		for (const auto &recipe: app.game->recipes.crafting) {

			auto &label = labels.emplace_back(recipe.output +
				(recipe.amount == 1? "" : " x " + niceDouble(recipe.amount)), Gtk::Align::START);
			std::vector<std::string> pieces;
			for (const auto &[name, amount]: recipe.inputs)
				pieces.push_back(name + " x " + niceDouble(amount));
			label.set_tooltip_text(join(pieces, "\n"));
			recipesBox.append(label);
		}
	}
}
