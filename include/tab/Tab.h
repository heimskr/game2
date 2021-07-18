#pragma once

#include <gtkmm-4.0/gtkmm.h>

namespace Game2 {
	class Tab {
		public:
			virtual Gtk::Widget & getWidget() = 0;
			virtual Glib::ustring getName() = 0;
			virtual ~Tab() {}
	};
}