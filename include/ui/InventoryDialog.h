#pragma once

#include <iostream>

#include "UpdatingDialog.h"
#include "App.h"

class InventoryDialog: public UpdatingDialog {
	public:
		std::vector<std::unique_ptr<Gtk::Widget>> widgets;
		std::unordered_map<std::string, Gtk::Label> nameLabels, amountLabels;
		std::unordered_map<std::string, Gtk::Button> buttons;
		Gtk::ScrolledWindow scrolled;
		Gtk::Grid grid;
		Gtk::Button cancel {"Cancel"};

		sigc::signal<void(Glib::ustring)> signal_submit() const { return signal_submit_; }

		InventoryDialog(const Glib::ustring &title, Gtk::Window &parent, bool modal = true):
		UpdatingDialog(title, parent, modal) {
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

			nameLabels.clear();
			amountLabels.clear();
			widgets.clear();

			auto lock = app->lockGame();

			unsigned row = 0;
			for (const auto &[name, amount]: app->game->inventory)
				insertRow(name, amount, row++);
		}

		void updateData() override {
			auto lock = app->lockGame();

			std::vector<std::string> removed;

			for (const auto &[name, label]: nameLabels)
				if (app->game->inventory.count(name) == 0)
					removed.push_back(name);

			for (const std::string &name: removed) {
				std::cout << "Removing " << name << "\n";
				grid.remove(nameLabels.at(name));
				grid.remove(amountLabels.at(name));
				grid.remove(buttons.at(name));
				nameLabels.erase(name);
				amountLabels.erase(name);
				buttons.erase(name);
			}

			unsigned row = 0;
			for (const auto &[name, amount]: app->game->inventory) {
				if (nameLabels.count(name) == 0) {
					grid.insert_row(row);
					insertRow(name, amount, row);
				} else {
					amountLabels.at(name).set_label(std::to_string(amount));
				}
				++row;
			}
		}

	protected:
		sigc::signal<void(Glib::ustring)> signal_submit_;

		void insertRow(const std::string &name, double amount, unsigned row) {
			auto *label = &nameLabels.emplace(name, Gtk::Label(name)).first->second;
			label->set_hexpand(true);
			label->set_halign(Gtk::Align::ALIGN_START);
			grid.attach(*label, 0, row);

			label = &amountLabels.emplace(name, Gtk::Label(std::to_string(amount))).first->second;
			label->set_halign(Gtk::Align::ALIGN_START);
			grid.attach(*label, 1, row);

			auto *button = &buttons.emplace(name, Gtk::Button("Select")).first->second;
			button->signal_clicked().connect([this, name] { submit(name); });
			grid.attach(*button, 2, row);
		}

		void submit(const std::string &resource) {
			signal_submit_.emit(resource);
			hide();
		}
};
