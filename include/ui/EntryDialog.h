#pragma once

#include <gtkmm-3.0/gtkmm.h>

class EntryDialog: public Gtk::Dialog {
	public:
		Gtk::Label label;
		Gtk::Entry entry;
		Gtk::ButtonBox buttons;
		Gtk::Button okay, cancel;

		EntryDialog(const Glib::ustring &title, Gtk::Window &parent, const Glib::ustring &text, bool modal = true):
		Dialog(title, parent, modal) {
			label.set_text(text);
			okay.set_label("OK");
			cancel.set_label("Cancel");
			buttons.add(cancel);
			buttons.add(okay);
			buttons.set_spacing(5);
			auto &area = *get_content_area();
			area.add(label);
			area.add(entry);
			area.add(buttons);
			area.set_spacing(5);
			area.set_margin_top(5);
			buttons.set_halign(Gtk::Align::ALIGN_END);
			
			show_all_children();
		}

		virtual ~EntryDialog() {}
};
