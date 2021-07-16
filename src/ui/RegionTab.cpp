#include "App.h"
#include "UI.h"
#include "ui/InventoryDialog.h"
#include "ui/RegionTab.h"

RegionTab::RegionTab(App &app_): app(app_), box(*getWidget<Gtk::Box>(app_, "regions_box")) {
	reset();
}

void RegionTab::reset() {
	removeChildren(box);
	update();
}

void RegionTab::update() {
	if (!app.game)
		return;

	auto lock = app.lockGame();

	Game &game = *app.game;

	auto region_ptr = game.currentRegionPointer();
	auto &region = *region_ptr;
	auto &pos = region.position;

	{
		Gtk::Label *label = getWidget<Gtk::Label>("region_name");
		label->set_text(region.name);

		label = getWidget<Gtk::Label>("position_label");
		label->set_text("Position: (" + std::to_string(pos.first) + ", " + std::to_string(pos.second) + ")");

		label = getWidget<Gtk::Label>("size_label");
		label->set_text("Size: " + std::to_string(region.size));
	}

	if (region_ptr != lastRegion) {
		lastRegion = region_ptr;
		removeChildren(box);
		widgets.clear();
		for (auto &[name, area]: region.areas) {
			Gtk::Expander *expander = new Gtk::Expander(area->name);
			widgets.emplace_back(expander);
			box.add(*expander);
			expander->set_margin_bottom(10);

			Gtk::Box *ebox = new Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL, 5);
			widgets.emplace_back(ebox);
			expander->add(*ebox);

			Gtk::Box *bbox = new Gtk::Box();
			bbox->set_spacing(5);
			widgets.emplace_back(bbox);
			ebox->add(*bbox);

			Gtk::Button *button = new Gtk::Button("Move");
			widgets.emplace_back(button);
			button->set_tooltip_text("Move a resource from your inventory into the area");
			bbox->add(*button);
			button->signal_clicked().connect([this] {
				auto dialock = app.lockDialog();
				auto *dialog = new InventoryDialog("Resource Selector", *app.mainWindow);
				app.dialog.reset(dialog);
				app.dialog->show();
			});

			button = new Gtk::Button("Resize");
			widgets.emplace_back(button);
			bbox->add(*button);
			button->set_halign(Gtk::Align::ALIGN_START);
			button->signal_clicked().connect([&] {

			});

			for (const auto &[rname, amount]: area->resources) {
				Gtk::Box *rbox = new Gtk::Box(Gtk::Orientation::ORIENTATION_HORIZONTAL, 5);
				widgets.emplace_back(rbox);
				ebox->add(*rbox);

				button = new Gtk::Button("Extract");
				widgets.emplace_back(button);
				rbox->add(*button);

				Gtk::Label *label = new Gtk::Label(rname + " x " + std::to_string(amount));
				widgets.emplace_back(label);
				label->set_halign(Gtk::Align::ALIGN_START);
				label->set_hexpand(true);
				rbox->add(*label);
			}
		}
		box.show_all_children();
	} else {

	}
}
