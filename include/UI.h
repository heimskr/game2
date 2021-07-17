#pragma once

#include <gtkmm-4.0/gtkmm.h>

void setMargins(Gtk::Widget &widget, int margin);

template <typename T>
void removeChildren(T &widget) {
	while (Gtk::Widget *child = widget.get_first_child())
		widget.remove(*child);
}

template <typename T>
void removeChildren(T *widget) {
	while (Gtk::Widget *child = widget->get_first_child())
		widget->remove(*child);
}
