#pragma once

#include <gtkmm-4.0/gtkmm.h>

struct NumericEntry: public Gtk::Entry {
	virtual void insert_text_vfunc(const Glib::ustring &text, int &position) override;
	virtual void on_insert_text(const Glib::ustring &text, int *position) override;
};
