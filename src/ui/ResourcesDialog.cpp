#include "App.h"
#include "ui/ResourcesDialog.h"

namespace Game2 {
	ResourcesDialog::ResourcesDialog(const Glib::ustring &title, Gtk::Window &parent, App &app_, bool modal):
	Dialog(title, parent, modal), app(app_) {
		set_default_size(300, 500);
		auto &area = *get_content_area();
		area.set_orientation(Gtk::Orientation::VERTICAL);
		area.set_spacing(5);
		area.set_margin_top(5);
		area.set_margin_start(5);
		area.set_margin_end(5);
		area.set_margin_bottom(5);
		cancel.set_halign(Gtk::Align::END);
		cancel.signal_clicked().connect(sigc::mem_fun(*this, &ResourcesDialog::hide));
		scrolled.set_size_request(-1, 150);
		resourcesBox.set_spacing(5);
		scrolled.set_vexpand(true);
		scrolled.set_child(resourcesBox);
		auto lock = app.lockGame();
		for (auto &[name, resource]: app.game->resources) {
			auto &label = labels.emplace_back(name, Gtk::Align::START);
			auto &gesture = gestures.emplace_back(Gtk::GestureClick::create());
			gesture->signal_pressed().connect([this, name](int, double, double) {
				done = true;
				signal_submit_.emit(name);
				hide();
			});
			label.add_controller(gesture);
			resourcesBox.append(label);
		}
		signal_hide().connect([this] {
			if (!done) {
				done = true;
				signal_submit_.emit("");
			}
		});
		signal_close_request().connect([this] {
			signal_submit_.emit("");
			return false;
		}, true);
		area.append(scrolled);
		area.append(cancel);
	}
}
