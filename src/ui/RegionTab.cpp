#include "App.h"
#include "UI.h"
#include "ui/InventoryDialog.h"
#include "ui/RegionTab.h"

RegionTab::RegionTab(App &app_): app(app_) {

	box.append(nameLabel);
	box.append(positionLabel);
	box.append(sizeLabel);
	box.set_spacing(5);
	setMargins(box, 5);

	for (Gtk::Label *label: {&positionLabel, &sizeLabel})
		label->set_halign(Gtk::Align::START);

	nameLabel.get_style_context()->add_class("big");

	app.gtkApp->signal_activate().connect([this] {
		update();
	});
}

void RegionTab::update() {
	if (!app.game)
		return;

	auto lock = app.lockGame();

	Game &game = *app.game;

	auto region_ptr = game.currentRegionPointer();
	auto &region = *region_ptr;
	auto &pos = region.position;

	// Let's hope no one uses markup in their region name.
	nameLabel.set_markup("<b>" + region.name + "</b>");
	positionLabel.set_text("Position: (" + std::to_string(pos.first) + ", " + std::to_string(pos.second) + ")");
	sizeLabel.set_text("Size: " + std::to_string(region.size));

	return;

	if (region_ptr != lastRegion) {
		lastRegion = region_ptr;
		removeChildren(box);
		widgets.clear();
		for (auto &[area_name, area]: region.areas) {
			Gtk::Expander *expander = new Gtk::Expander(area->name);
			widgets.emplace_back(expander);
			box.append(*expander);
			expander->set_margin_bottom(10);

			Gtk::Box *ebox = new Gtk::Box(Gtk::Orientation::VERTICAL, 5);
			widgets.emplace_back(ebox);
			expanderBoxes.emplace(area_name, ebox);
			expander->set_child(*ebox);

			Gtk::Box *bbox = new Gtk::Box();
			bbox->set_spacing(5);
			widgets.emplace_back(bbox);
			ebox->append(*bbox);

			Gtk::Button *button = new Gtk::Button("Move");
			widgets.emplace_back(button);
			button->set_tooltip_text("Move a resource from your inventory into the area");
			bbox->append(*button);
			button->signal_clicked().connect([this] {
				auto dialock = app.lockDialog();
				auto *dialog = new InventoryDialog("Resource Selector", *app.mainWindow);
				app.dialog.reset(dialog);
				dialog->signal_submit().connect([&](const Glib::ustring &str) {
					std::cout << "Selected " << str << "\n";
				});
				app.dialog->show();
			});

			button = new Gtk::Button("Resize");
			widgets.emplace_back(button);
			bbox->append(*button);
			button->set_halign(Gtk::Align::START);
			button->signal_clicked().connect([&] {

			});

			resourceSets[area_name].clear();
			boxMaps[area_name].clear();

			for (const auto &[resource_name, amount]: area->resources) {
				resourceSets[area_name].insert(resource_name);
				Gtk::Box *rbox = new Gtk::Box(Gtk::Orientation::HORIZONTAL, 5);
				widgets.emplace_back(rbox);
				ebox->append(*rbox);
				boxMaps[area_name][resource_name] = rbox;

				button = new Gtk::Button("Extract");
				widgets.emplace_back(button);
				rbox->append(*button);

				Gtk::Label *label = new Gtk::Label(resource_name + " x " + std::to_string(amount));
				widgets.emplace_back(label);
				label->set_halign(Gtk::Align::START);
				label->set_hexpand(true);
				rbox->append(*label);
			}
		}
	} else {
		for (auto &[area_name, area]: region.areas) {
			auto &resource_set = resourceSets[area_name];

			std::list<std::string> removed;

			for (const std::string &resource_name: resource_set)
				if (area->resources.count(resource_name) == 0)
					removed.push_back(resource_name);

			auto &map = boxMaps.at(area_name);
			Gtk::Box &ebox = *expanderBoxes.at(area_name);
			for (const std::string &resource_name: removed) {
				// ebox.insert_child_after();
				resource_set.erase(resource_name);
			}

			for (const auto &[resource_name, amount]: area->resources) {

			}
		}
	}
}
