#include <iostream>

#include "ui/NumericEntry.h"

static void (*old_insert_text)(GtkEditable *, const char *, int length, int *position) = nullptr;
static void (*old_activate)(GtkEntry *) = nullptr;

static void new_insert_text(GtkEditable *editable, const char *text, int length, int *position) {
	Glib::ustring str(text, length);
	for (auto ch: str)
		if (!Glib::Unicode::isdigit(ch) && ch != '.')
			return;
	old_insert_text(editable, text, length, position);
}

NumericEntry::NumericEntry() {
	signal_map().connect([this] {
		Gtk::Text *text = dynamic_cast<Gtk::Text *>(get_first_child());

		GtkEditableInterface *interface = GTK_EDITABLE_GET_IFACE(GTK_EDITABLE(text->gobj()));
		if (!old_insert_text)
			old_insert_text = interface->do_insert_text;
		interface->do_insert_text = new_insert_text;

		auto c = GTK_ENTRY_GET_CLASS(GTK_ENTRY(gobj()));
		if (!old_activate)
			old_activate = c->activate;
		c->activate = [](GtkEntry *c_entry) {
			auto *entry = Glib::wrap(c_entry, true);
			if (auto *numeric_entry = dynamic_cast<NumericEntry *>(entry))
				numeric_entry->signal_activate().emit();
			if (old_activate)
				old_activate(c_entry);
		};
	});
}
