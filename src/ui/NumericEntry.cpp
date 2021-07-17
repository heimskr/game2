#include <iostream>

#include "ui/NumericEntry.h"

void NumericEntry::insert_text_vfunc(const Glib::ustring &text, int &position) {
	std::cout << "insert_text_vfunc(" << text << ")\n";
	for (auto ch: text)
		if (!Glib::Unicode::isdigit(ch))
			return;
	Gtk::Entry::on_insert_text(text, &position);
}

void NumericEntry::on_insert_text(const Glib::ustring &text, int *position) {
	std::cout << "on_insert_text(" << text << ")\n";
	for (auto ch: text)
		if (!Glib::Unicode::isdigit(ch))
			return;
	Gtk::Entry::on_insert_text(text, position);
}
