#pragma once

#include "Toolkit.h"

namespace Game2 {
	class Game;

	/** Represents a dialog that derives its data from a Game. */
	class UpdatingDialog: public Gtk::Dialog {
		public:
			using Gtk::Dialog::Dialog;
			virtual void updateData() = 0;
	};
}
