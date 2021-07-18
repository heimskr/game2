#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <iostream>
#include <optional>
#include <vector>

#include "Processor.h"

namespace Game2 {
	class App;

	class ProcessorsDialog: public Gtk::Dialog {
		public:
			Gtk::Button cancel {"Cancel"};
			Gtk::ScrolledWindow scrolled;
			Gtk::Box processorsBox {Gtk::Orientation::VERTICAL};
			sigc::signal<void(std::optional<Processor::Type>)> signal_submit() const { return signal_submit_; }

			ProcessorsDialog(const Glib::ustring &title, Gtk::Window &parent, App &, bool modal = true);

		private:
			App &app;
			bool done = false;
			std::vector<Gtk::Label> labels;
			std::vector<Glib::RefPtr<Gtk::GestureClick>> gestures;
			sigc::signal<void(std::optional<Processor::Type>)> signal_submit_;
	};
}
