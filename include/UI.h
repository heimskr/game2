#pragma once

#include "App.h"

template <typename T>
T * getWidget(const char *name) {
	T *widget;
	app->builder->get_widget<T>(name, widget);
	return widget;
}

template <typename T>
void setMargins(T &widget, int margin) {
	widget.set_margin_top(margin);
	widget.set_margin_right(margin);
	widget.set_margin_bottom(margin);
	widget.set_margin_left(margin);
}

template <typename T>
void removeChildren(T &widget) {
	for (Gtk::Widget *child: widget.get_children())
		widget.remove(*child);
}
