#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <vector>

namespace Game2 {
	class App;

	class RecipesDialog: public Gtk::Dialog {
		public:
			Gtk::ScrolledWindow scrolled;
			Gtk::Box recipesBox {Gtk::Orientation::VERTICAL};

			RecipesDialog(const Glib::ustring &title, Gtk::Window &parent, App &, bool modal = true);

		private:
			App &app;
			std::vector<Gtk::Label> labels;
	};
}
