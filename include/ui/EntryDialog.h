#pragma once

#include <gtkmm-3.0/gtkmm.h>

class EntryDialog: public Gtk::Dialog {
	public:
		Gtk::Label label;
		Gtk::Entry entry;
		Gtk::ButtonBox buttons;
		Gtk::Button okay, cancel;

		sigc::signal<void(Glib::ustring)> signal_submit() const { return signal_submit_; }

		EntryDialog(const Glib::ustring &title, Gtk::Window &parent, const Glib::ustring &text, bool modal = true):
		Dialog(title, parent, modal) {
			int width, height;
			get_default_size(width, height);
			set_default_size(300, height);
			label.set_text(text);
			label.set_halign(Gtk::Align::ALIGN_START);
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
			area.set_margin_left(5);
			area.set_margin_right(5);
			buttons.set_halign(Gtk::Align::ALIGN_END);
			entry.signal_activate().connect(sigc::mem_fun(this, &EntryDialog::submit));
			okay.signal_clicked().connect(sigc::mem_fun(this, &EntryDialog::submit));
			cancel.signal_clicked().connect(sigc::mem_fun(this, &EntryDialog::hide));
			show_all_children();
		}

		EntryDialog & set_text(const Glib::ustring &str) { entry.set_text(str); return *this; }

		virtual ~EntryDialog() {}

	protected:
		sigc::signal<void(Glib::ustring)> signal_submit_;

		void submit() {
			signal_submit_.emit(entry.get_text());
			hide();
		}
};
