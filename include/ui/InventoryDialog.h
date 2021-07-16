#pragma once

#include <gtkmm-3.0/gtkmm.h>
#include "App.h"

class InventoryDialog: public Gtk::Dialog {
	public:
		std::vector<std::unique_ptr<Gtk::Widget>> widgets;
		Gtk::ScrolledWindow scrolled;
		Gtk::Grid grid;
		Gtk::Button cancel {"Cancel"};

		sigc::signal<void(Glib::ustring)> signal_submit() const { return signal_submit_; }

		InventoryDialog(const Glib::ustring &title, Gtk::Window &parent, bool modal = true):
		Dialog(title, parent, modal) {
			set_default_size(500, -1);

			get_content_area()->add(scrolled);
			scrolled.add(grid);
			scrolled.set_size_request(-1, 500);
			get_content_area()->add(cancel);
			grid.set_row_spacing(5);
			grid.set_column_spacing(5);
			grid.set_margin_left(5);
			grid.set_margin_right(5);

			updateContent();

			cancel.signal_clicked().connect(sigc::mem_fun(this, &InventoryDialog::hide));
			show_all_children();
		}

		void updateContent() {
			for (Gtk::Widget *child: grid.get_children())
				grid.remove(*child);

			widgets.clear();

			auto lock = app->lockGame();

			unsigned row = 0;

			for (const auto &pair: app->game->inventory) {
				const auto &name = pair.first;
				const auto &amount = pair.second;
				
				auto *label = new Gtk::Label(name);
				widgets.emplace_back(label);
				label->set_hexpand(true);
				label->set_halign(Gtk::Align::ALIGN_START);
				grid.attach(*label, 0, row);

				label = new Gtk::Label(std::to_string(amount));
				widgets.emplace_back(label);
				label->set_halign(Gtk::Align::ALIGN_START);
				grid.attach(*label, 1, row);

				auto *button = new Gtk::Button("Select");
				widgets.emplace_back(button);
				button->signal_clicked().connect([this, name] { submit(name); });
				grid.attach(*button, 2, row);

				++row;
			}
		}

	protected:
		sigc::signal<void(Glib::ustring)> signal_submit_;

		void submit(const std::string &resource) {
			signal_submit_.emit(resource);
			hide();
		}
};
