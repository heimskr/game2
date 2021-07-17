#include "UI.h"

void setMargins(Gtk::Widget &widget, int margin) {
	widget.set_margin_top(margin);
	widget.set_margin_end(margin);
	widget.set_margin_bottom(margin);
	widget.set_margin_start(margin);
}
