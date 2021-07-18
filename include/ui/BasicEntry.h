#pragma once

#include <gtkmm-4.0/gtkmm.h>

namespace Game2 {
	/** Why on earth was signal_activate() removed from Gtk::Entry? It's not as if I can set a button as default
	 *  anymore. */
	class BasicEntry: public Gtk::Entry {
		public:
			BasicEntry();
			sigc::signal<void()> signal_activate() const { return signal_activate_; }

		private:
			sigc::signal<void()> signal_activate_;

	};
}
