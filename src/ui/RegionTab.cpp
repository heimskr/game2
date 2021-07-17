#include "App.h"
#include "UI.h"
#include "ui/EntryDialog.h"
#include "ui/InventoryDialog.h"
#include "ui/NumericEntry.h"
#include "ui/RegionTab.h"

RegionTab::Rbox::Rbox(const std::string &resource_name, double amount, std::function<void()> on_click):
Box(Gtk::Orientation::HORIZONTAL, 5), onClick(on_click) {
	append(extractButton);
	append(label);
	updateLabel(resource_name, amount);
	label.set_halign(Gtk::Align::START);
	label.set_hexpand(true);
	extractButton.signal_clicked().connect(on_click);
}

void RegionTab::Rbox::updateLabel(const std::string &resource_name, double amount) {
	label.set_text(resource_name + " x " + std::to_string(amount));
}

RegionTab::RegionTab(App &app_): app(app_) {
	box.append(nameLabel);
	box.append(positionLabel);
	box.append(sizeLabel);
	box.set_spacing(5);
	setMargins(box, 5);

	positionLabel.set_halign(Gtk::Align::START);
	sizeLabel.set_halign(Gtk::Align::START);
	nameLabel.add_css_class("big");

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
	positionLabel.set_text("Position: " + std::string(pos));
	sizeLabel.set_text("Size: " + std::to_string(region.size));

	// return;

	if (region_ptr != lastRegion) {
		lastRegion = region_ptr;

		for (Gtk::Expander &expander: expanders)
			box.remove(expander);
		expanders.clear();

		widgets.clear();

		for (const auto &pair: region.areas) {
			const std::string &area_name = pair.first;
			std::shared_ptr<Area> area = pair.second;
			Gtk::Expander &expander = expanders.emplace_back(area->name);
			box.append(expander);
			expander.set_margin_bottom(10);

			auto ebox = std::make_shared<Gtk::Box>(Gtk::Orientation::VERTICAL, 5);
			widgets.push_back(ebox);
			expanderBoxes.emplace(area_name, ebox);
			expander.set_child(*ebox);

			Gtk::Box *bbox = new Gtk::Box(Gtk::Orientation::HORIZONTAL, 5);
			widgets.emplace_back(bbox);
			ebox->append(*bbox);

			Gtk::Button *button = new Gtk::Button("Move");
			widgets.emplace_back(button);
			button->set_tooltip_text("Move a resource from your inventory into the area");
			bbox->append(*button);
			button->signal_clicked().connect([this, area] {
				auto *dialog = new InventoryDialog("Resource Selector", *app.mainWindow);
				app.dialog.reset(dialog);
				dialog->signal_submit().connect([this, area](const Glib::ustring &resource_name) {
					app.delay([this, resource_name, area]() {
						auto *dialog = new EntryDialog<NumericEntry>("Amount", *app.mainWindow,
							"Amount of " + resource_name + " to transfer:");
						dialog->signal_submit().connect([this, resource_name, area](const Glib::ustring &amount_text) {
							double amount;
							try {
								amount = parseDouble(amount_text);
							} catch (std::invalid_argument &) {
								app.delay([this] { app.error("Invalid amount."); });
								return;
							}
							if (insert(area, resource_name, amount))
								update();
						});
						app.dialog.reset(dialog);
						app.dialog->show();
					});
				});
				app.dialog->show();
			});

			button = new Gtk::Button("Resize");
			widgets.emplace_back(button);
			bbox->append(*button);
			button->set_halign(Gtk::Align::START);
			button->signal_clicked().connect([&] {});

			resourceSets[area_name].clear();
			boxMaps[area_name].clear();

			for (const auto &[resource_name, amount]: area->resources) {
				resourceSets[area_name].insert(resource_name);
				ebox->append(*(boxMaps[area_name][resource_name] = std::make_shared<Rbox>(resource_name, amount, [] {
					std::cout << "Clicked 1.\n";
				})));
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
				ebox.remove(*map.at(resource_name));
				map.erase(resource_name);
				resource_set.erase(resource_name);
			}

			Rbox *last_rbox = nullptr;

			for (const auto &[resource_name, amount]: area->resources) {
				std::shared_ptr<Rbox> rbox;
				if (resource_set.count(resource_name) == 0) {
					rbox = std::make_shared<Rbox>(resource_name, amount, [] {
						std::cout << "Clicked 2.\n";
					});
					if (!last_rbox) {
						map.emplace(resource_name, rbox);
						ebox.prepend(*rbox);
					} else
						rbox->insert_after(ebox, *last_rbox);
					last_rbox = rbox.get();
					resource_set.insert(resource_name);
				} else
					rbox = map.at(resource_name);

				rbox->updateLabel(resource_name, amount);
			}
		}
	}
}

bool RegionTab::insert(std::shared_ptr<Area> area, const Glib::ustring &resource_name, double amount) {
	try {
		if (amount <= 0) {
			app.alert("Error: Invalid amount.");
			return false;
		}

		if (app.game->inventory.count(resource_name) == 0) {
			app.alert("Error: You don't have any of that resource.");
			return false;
		}

		double &in_inventory = app.game->inventory.at(resource_name);
		if (ltna(in_inventory, amount)) {
			app.alert("Error: You don't have enough of that resource.");
			return false;
		}

		if ((in_inventory -= amount) < Resource::MIN_AMOUNT)
			app.game->inventory.erase(resource_name);

		area->resources[resource_name] += amount;
		return true;
	} catch (const std::exception &err) {
		fprintf(stderr, "??? %s\n", err.what());
		app.error("??? " + std::string(err.what()));
		return false;
	}
}
