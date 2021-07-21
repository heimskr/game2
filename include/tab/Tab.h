#pragma once

#include "Toolkit.h"

namespace Game2 {
	class Tab {
		public:
			virtual Gtk::Widget & getWidget() = 0;
			virtual Glib::ustring getName() = 0;
			virtual void onFocus() {}
			virtual void onBlur() {}
			virtual ~Tab() {}
	};
}
