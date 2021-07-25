#pragma once

#include "Toolkit.h"
#include <iostream>
#include <optional>
#include <vector>

namespace Game2 {
	class AppWindow;

	class ResourcesDialog: public Gtk::Dialog {
		public:
			ResourcesDialog(const Glib::ustring &title, Gtk::Window &parent, AppWindow &, bool modal = true);

			sigc::signal<void(const std::string &)> signal_submit() const { return signal_submit_; }

		private:
			AppWindow &appWindow;
			bool done = false;
			Gtk::Button cancel {"Cancel"};
			Gtk::ScrolledWindow scrolled;
			Gtk::Box resourcesBox {Gtk::Orientation::VERTICAL};
			std::vector<Gtk::Label> labels;
			std::vector<Glib::RefPtr<Gtk::GestureClick>> gestures;
			sigc::signal<void(const std::string &)> signal_submit_;
	};
}
