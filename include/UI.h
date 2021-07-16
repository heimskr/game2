#pragma once

#include "App.h"

template <typename T>
void setMargins(T &widget, int margin) {
	widget.set_margin_top(margin);
	widget.set_margin_end(margin);
	widget.set_margin_bottom(margin);
	widget.set_margin_start(margin);
}

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
