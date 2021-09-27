#include "Game.h"
#include "UI.h"
#include "tab/ResourcesTab.h"
#include "ui/AppWindow.h"

namespace Game2 {
	ResourcesTab::ResourcesTab(AppWindow &app_window): appWindow(app_window) {
		scrolled.set_child(treeView);
		scrolled.set_vexpand(true);
		treeModel = Gtk::ListStore::create(columns);
		treeView.set_model(treeModel);
		appendColumn(treeView, "Resource", columns.resource);
		appendColumn(treeView, "Base Price", columns.basePrice);
		treeView.get_column(0)->set_resizable(true);
		treeView.get_column(1)->set_resizable(true);
		reset();
	}

	void ResourcesTab::reset() {
		treeModel->clear();
		if (!appWindow.game)
			return;
		auto lock = appWindow.lockGame();
		for (const auto &[name, resource]: appWindow.game->resources) {
			auto row = treeModel->append();
			(*row)[columns.resource] = name;
			(*row)[columns.basePrice] = resource.basePrice;
		}
	}
}
