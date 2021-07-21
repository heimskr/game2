#include "App.h"
#include "UI.h"
#include "ui/RefineryModesDialog.h"

namespace Game2 {
	RefineryModesDialog::RefineryModesDialog(const Glib::ustring &title, Gtk::Window &parent, App &app_, bool modal):
	Dialog(title, parent, modal), app(app_) {
		set_default_size(300, 300);
		scrolled.set_vexpand(true);
		scrolled.set_child(modesBox);
		setMargins(modesBox, 5);
		modesBox.set_spacing(5);
		set_child(scrolled);
		for (RefineryMode mode: refineryModes) {
			auto &label = labels.emplace_back(stringify(mode), Gtk::Align::START);
			auto &gesture = gestures.emplace_back(Gtk::GestureClick::create());
			gesture->signal_pressed().connect([this, mode](int, double, double) {
				signal_submit_.emit(mode);
				hide();
			});
			label.add_controller(gesture);
			modesBox.append(label);
		}
	}
}
