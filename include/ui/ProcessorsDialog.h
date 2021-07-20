#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <iostream>
#include <optional>
#include <vector>

namespace Game2 {
	class App;
	class Processor;

	class ProcessorsDialog: public Gtk::Dialog {
		public:
			Gtk::Button cancel {"Cancel"};
			Gtk::ScrolledWindow scrolled;
			Gtk::Grid grid;
			sigc::signal<void(std::shared_ptr<Processor>)> signal_submit() const { return signal_submit_; }

			ProcessorsDialog(const Glib::ustring &title, Gtk::Window &parent, App &, bool modal = true);

		private:
			App &app;
			bool done = false;
			std::vector<Gtk::Label> labels;
			std::vector<Glib::RefPtr<Gtk::GestureClick>> gestures;
			sigc::signal<void(std::shared_ptr<Processor>)> signal_submit_;
	};
}
