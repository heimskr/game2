#include "App.h"
#include "ui/ProcessorsDialog.h"

namespace Game2 {
	ProcessorsDialog::ProcessorsDialog(const Glib::ustring &title, Gtk::Window &parent, App &app_, bool modal):
	Dialog(title, parent, modal), app(app_) {
		set_default_size(300, -1);
		auto &area = *get_content_area();
		area.set_orientation(Gtk::Orientation::VERTICAL);
		area.set_spacing(5);
		area.set_margin_top(5);
		area.set_margin_start(5);
		area.set_margin_end(5);
		area.set_margin_bottom(5);
		cancel.set_halign(Gtk::Align::END);
		cancel.signal_clicked().connect(sigc::mem_fun(*this, &ProcessorsDialog::hide));
		scrolled.set_size_request(-1, 150);
		grid.set_row_spacing(5);
		grid.set_column_spacing(5);
		scrolled.set_vexpand(true);
		scrolled.set_child(grid);
		auto lock = app.lockGame();
		auto &name_label = labels.emplace_back("Name");
		name_label.add_css_class("table-header");
		name_label.set_xalign(0);
		name_label.set_hexpand(true);
		grid.attach(name_label, 0, 0);
		auto &type_label = labels.emplace_back("Type");
		type_label.add_css_class("table-header");
		type_label.set_xalign(0);
		grid.attach(type_label, 1, 0);
		int row = 0;
		for (auto &processor: app.game->processors) {
			auto &name_label = labels.emplace_back(processor->name);
			auto &name_gesture = gestures.emplace_back(Gtk::GestureClick::create());
			name_gesture->signal_pressed().connect([this, processor](int, double, double) {
				signal_submit_.emit(processor);
			});
			name_label.add_controller(name_gesture);
			name_label.set_xalign(0);
			grid.attach(name_label, 0, ++row);

			auto &type_label = labels.emplace_back(Processor::typeName(processor->getType()));
			auto &type_gesture = gestures.emplace_back(Gtk::GestureClick::create());
			type_gesture->signal_pressed().connect([this, processor](int, double, double) {
				signal_submit_.emit(processor);
			});
			type_label.add_controller(type_gesture);
			type_label.set_xalign(0);
			grid.attach(type_label, 1, row);
			
		}
		signal_hide().connect([this] {
			if (!done) {
				done = true;
				signal_submit_.emit(nullptr);
			}
		});
		signal_close_request().connect([this] {
			signal_submit_.emit(nullptr);
			return false;
		}, true);
		area.append(scrolled);
		area.append(cancel);
	}
}
