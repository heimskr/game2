#pragma once

#include "Toolkit.h"
#include <vector>

#include "defs/RefineryMode.h"

namespace Game2 {
	class App;
	struct CraftingRecipe;

	class RefineryModesDialog: public Gtk::Dialog {
		public:
			Gtk::ScrolledWindow scrolled;
			Gtk::Box modesBox {Gtk::Orientation::VERTICAL};

			RefineryModesDialog(const Glib::ustring &title, Gtk::Window &parent, App &, bool modal = true);

			sigc::signal<void(RefineryMode)> signal_submit() const { return signal_submit_; }

		private:
			App &app;
			std::vector<Gtk::Label> labels;
			std::vector<Glib::RefPtr<Gtk::GestureClick>> gestures;
			sigc::signal<void(RefineryMode)> signal_submit_;
	};
}
