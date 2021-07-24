#pragma once

#include "Toolkit.h"
#include <iostream>
#include <optional>
#include <vector>

#include "Processor.h"

namespace Game2 {
	class AppWindow;

	class ProcessorTypeDialog: public Gtk::Dialog {
		public:
			Gtk::Button cancel {"Cancel"};
			Gtk::ScrolledWindow scrolled;
			Gtk::Box processorsBox {Gtk::Orientation::VERTICAL};
			sigc::signal<void(std::optional<Processor::Type>)> signal_submit() const { return signal_submit_; }

			ProcessorTypeDialog(const Glib::ustring &title, Gtk::Window &parent, AppWindow &, bool modal = true);

		private:
			AppWindow &appWindow;
			bool done = false;
			std::vector<Gtk::Label> labels;
			std::vector<Glib::RefPtr<Gtk::GestureClick>> gestures;
			sigc::signal<void(std::optional<Processor::Type>)> signal_submit_;
	};
}
