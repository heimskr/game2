#include <iostream>
#include "ui/BasicEntry.h"

static void (*old_activate)(GtkEntry *);
static bool old_activate_set = false;

namespace Game2 {
	BasicEntry::BasicEntry() {
		signal_map().connect([this] {
			auto c = GTK_ENTRY_GET_CLASS(GTK_ENTRY(gobj()));
			if (!old_activate_set) {
				old_activate_set = true;
				old_activate = c->activate;
				c->activate = +[](GtkEntry *c_entry) {
					auto *entry = Glib::wrap(c_entry, true);
					if (auto *basic_entry = dynamic_cast<BasicEntry *>(entry))
						basic_entry->signal_activate().emit();
					if (old_activate)
						old_activate(c_entry);
				};
			}
		});
	}
}
