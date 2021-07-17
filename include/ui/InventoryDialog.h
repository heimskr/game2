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

		InventoryDialog(const Glib::ustring &title, Gtk::Window &parent, bool modal = true);;

		void updateContent();
		void updateData() override;

	protected:
		sigc::signal<void(Glib::ustring)> signal_submit_;
		void insertRow(const std::string &name, double amount, unsigned row);
		void submit(const std::string &resource);
};
