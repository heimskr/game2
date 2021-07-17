#pragma once

#include <gtkmm-4.0/gtkmm.h>

struct NumericEntry: public Gtk::Entry {
	void on_insert_text(const Glib::ustring &text, int *position) override {
		for (auto ch: text)
			if (!Glib::Unicode::isdigit(ch))
				return;
        Gtk::Entry::on_insert_text(text, position);
	}
};
