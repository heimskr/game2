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
		processorsBox.set_spacing(5);
		scrolled.set_child(processorsBox);
		for (Processor::Type type: Processor::TYPES) {
			auto &label = labels.emplace_back(Processor::typeName(type), Gtk::Align::START);
			std::vector<std::string> pieces;
			for (const auto &[name, amount]: app.game->processorCosts.at(type))
				pieces.push_back(name + " x " + niceDouble(amount));
			label.set_tooltip_text(join(pieces, "\n"));
			auto &gesture = gestures.emplace_back(Gtk::GestureClick::create());
			gesture->signal_pressed().connect([this, type](int, double, double) {
				done = true;
				signal_submit_.emit(type);
				hide();
			});
			label.add_controller(gesture);
			processorsBox.append(label);
		}
		signal_hide().connect([this] {
			if (!done) {
				done = true;
				signal_submit_.emit(std::nullopt);
			}
		});
		signal_close_request().connect([this] {
			signal_submit_.emit(std::nullopt);
			return false;
		}, true);
		area.append(scrolled);
		area.append(cancel);
	}
}
