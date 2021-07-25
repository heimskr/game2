#include "App.h"
#include "Game.h"
#include "UI.h"
#include "Util.h"
#include "tab/HelpTab.h"
#include "ui/AppWindow.h"

namespace Game2 {
	HelpTab::HelpTab(AppWindow &app_window): appWindow(app_window) {
		scrolled.set_child(textWidget);
		scrolled.set_hexpand(true);
		scrolled.set_vexpand(true);
		textWidget.set_text(App::get_text("/com/heimskr/game2/help.txt"));
		textWidget.set_halign(Gtk::Align::START);
		textWidget.set_valign(Gtk::Align::START);
		textWidget.set_selectable(true);
		textWidget.set_wrap(true);
		textWidget.set_focusable(false);
		textWidget.set_hexpand(true);
		textWidget.set_vexpand(true);
		setMargins(textWidget, 10);
	}
}
