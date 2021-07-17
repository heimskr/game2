#pragma once

#include <gtkmm-4.0/gtkmm.h>

class NumericEntry: public Gtk::Entry {
	protected:
		virtual void insert_text_vfunc(const Glib::ustring &text, int &position) override;
		virtual void on_insert_text(const Glib::ustring &text, int *position) override;
};
