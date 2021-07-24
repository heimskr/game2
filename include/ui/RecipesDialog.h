#pragma once

#include "Toolkit.h"
#include <vector>

namespace Game2 {
	class AppWindow;
	struct CraftingRecipe;

	class RecipesDialog: public Gtk::Dialog {
		public:
			Gtk::ScrolledWindow scrolled;
			Gtk::Box recipesBox {Gtk::Orientation::VERTICAL};

			RecipesDialog(const Glib::ustring &title, Gtk::Window &parent, AppWindow &, bool modal = true);

			sigc::signal<void(const CraftingRecipe &)> signal_submit() const { return signal_submit_; }

		private:
			AppWindow &appWindow;
			std::vector<Gtk::Label> labels;
			std::vector<Glib::RefPtr<Gtk::GestureClick>> gestures;
			sigc::signal<void(const CraftingRecipe &)> signal_submit_;
	};
}
