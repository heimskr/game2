#pragma once

#include "Toolkit.h"
#include <iostream>
#include <optional>
#include <vector>

namespace Game2 {
	class App;

	class ResourcesDialog: public Gtk::Dialog {
		public:
			Gtk::Button cancel {"Cancel"};
			Gtk::ScrolledWindow scrolled;
			Gtk::Box resourcesBox {Gtk::Orientation::VERTICAL};

			ResourcesDialog(const Glib::ustring &title, Gtk::Window &parent, App &, bool modal = true);

			sigc::signal<void(std::string)> signal_submit() const { return signal_submit_; }

		private:
			App &app;
			bool done = false;
			std::vector<Gtk::Label> labels;
			std::vector<Glib::RefPtr<Gtk::GestureClick>> gestures;
			sigc::signal<void(std::string)> signal_submit_;
	};
}
