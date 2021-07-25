#include "Game.h"
#include "NameGen.h"
#include "UI.h"
#include "tab/RegionTab.h"
#include "ui/AppWindow.h"
#include "ui/BasicEntry.h"
#include "ui/EntryDialog.h"
#include "ui/InventoryDialog.h"
#include "ui/NumericEntry.h"

namespace Game2 {
	RegionTab::Rbox::Rbox(AppWindow &app_window, std::shared_ptr<Area> area_, const std::string &resource_name,
	                      double amount, Extraction *extraction_, std::list<Extraction>::iterator extraction_iter):
	Box(Gtk::Orientation::HORIZONTAL, 5), appWindow(app_window), area(area_), extraction(extraction_),
	extractionIter(extraction_iter) {
		append(extractButton);
		append(labelBox);
		labelBox.append(mainLabel);
		labelBox.append(extractionLabel);
		labelBox.set_hexpand(true);
		updateLabel(resource_name, amount);
		mainLabel.set_halign(Gtk::Align::START);
		extractionLabel.set_halign(Gtk::Align::START);
		extractionLabel.add_css_class("red");
		extractionGesture = Gtk::GestureClick::create();
		extractionGesture->signal_pressed().connect([this](int count, double, double) {
			if (extraction && count == 2) {
				auto lock = appWindow.lockGame();
				if (extractionIter == appWindow.game->extractions.end())
					return;
				const std::string resource_name = extraction->resourceName;
				appWindow.game->extractions.erase(extractionIter);
				extraction = nullptr;
				extractionIter = appWindow.game->extractions.end();
				extractionLabel.set_text("");
				extractionLabel.hide();
			}
		});
		extractionLabel.add_controller(extractionGesture);
		if (!area->playerOwned)
			extractButton.set_sensitive(false);
		extractButton.signal_clicked().connect([this, resource_name] {
			if (extraction)
				return;
			auto *dialog = new EntryDialog<NumericEntry>("", appWindow, "Amount to extract:");
			appWindow.dialog.reset(dialog);
			dialog->signal_submit().connect([this, resource_name](const Glib::ustring &str) {
				double chosen;
				try {
					chosen = parseDouble(str);
					if (ltna(chosen, 0))
						throw std::invalid_argument("Invalid amount.");
				} catch (std::invalid_argument &) {
					appWindow.delay([this] { appWindow.error("Invalid amount."); });
					return;
				}
				if (ltna(0, chosen)) {
					if (area->resources.count(resource_name) == 0 || ltna(area->resources.at(resource_name), chosen)) {
						appWindow.delay([this] { appWindow.error("Not enough of that resource is available."); });
						return;
					}
				}
				appWindow.delay([this, resource_name, chosen] {
					auto *dialog = new EntryDialog<NumericEntry>("", appWindow, "Minimum amount in area:");
					appWindow.dialog.reset(dialog);
					dialog->signal_submit().connect([this, resource_name, chosen](const Glib::ustring &str) {
						double minimum;
						try {
							minimum = parseDouble(str);
							if (ltna(minimum, 0))
								throw std::invalid_argument("Invalid minimum.");
						} catch (std::invalid_argument &) {
							appWindow.delay([this] { appWindow.error("Invalid minimum."); });
							return;
						}
						extraction = &appWindow.game->extract(*area, resource_name, chosen, minimum);
						extractionIter = std::next(appWindow.game->extractions.end(), -1);
					});
					appWindow.dialog->show();
				});
			});
			appWindow.dialog->show();
		});
	}

	void RegionTab::Rbox::updateLabel(const std::string &resource_name, double amount) {
		mainLabel.set_text(resource_name + " x " + niceDouble(amount));
		if (extraction) {
			extractionLabel.set_text("- " + niceDouble(extraction->rate) + "/s");
			extractionLabel.show();
		} else
			extractionLabel.hide();
	}

	RegionTab::RegionTab(AppWindow &app_window): appWindow(app_window) {
		box.append(nameLabel);
		labelBox.append(positionLabel);
		labelBox.append(sizeLabel);
		box.append(labelBox);
		setMargins(box, 5);
		scrolled.set_child(expandersBox);
		scrolled.set_vexpand(true);
		box.append(scrolled);
		box.set_spacing(5);

		positionLabel.set_halign(Gtk::Align::START);
		sizeLabel.set_halign(Gtk::Align::START);
		nameLabel.add_css_class("big");
	}

	void RegionTab::onFocus() {
		if (!appWindow.game)
			return;

		renameButton = std::make_unique<Gtk::Button>();
		renameButton->set_icon_name("document-edit-symbolic");
		renameButton->set_tooltip_text("Rename region");

		deleteButton = std::make_unique<Gtk::Button>();
		deleteButton->set_icon_name("edit-delete-symbolic");
		deleteButton->set_tooltip_text("Delete region");

		renameButton->signal_clicked().connect([this] {
			auto *dialog = new EntryDialog<BasicEntry>("Rename Region", appWindow, "New region name:");
			appWindow.dialog.reset(dialog);
			dialog->set_text(NameGen::makeRandomLanguage().makeName());
			dialog->signal_submit().connect([&](Glib::ustring str) {
				auto lock = appWindow.lockGame();
				appWindow.game->updateName(appWindow.game->currentRegion(), str);
				appWindow.regionTab->update();
				appWindow.update();
			});
			appWindow.dialog->show();
		});

		deleteButton->signal_clicked().connect([this] {
			auto lock = appWindow.lockGame();
			Game &game = *appWindow.game;
			if (1 < game.regions.size()) {
				auto *dialog = new Gtk::MessageDialog(appWindow, "Are you sure you want to delete " +
					game.currentRegion().name + "?", false, Gtk::MessageType::QUESTION, Gtk::ButtonsType::OK_CANCEL,
					true);
				appWindow.dialog.reset(dialog);
				appWindow.dialog->signal_response().connect([&](int response) {
					auto lock = appWindow.lockGame();
					if (response == Gtk::ResponseType::OK) {
						game.erase(game.currentRegion());
						appWindow.update();
					}
					appWindow.dialog->hide();
				});
				appWindow.dialog->show();
			} else {
				appWindow.error("Can't delete region: no other region exists");
			}
		});

		appWindow.header->pack_start(*renameButton);
		appWindow.header->pack_start(*deleteButton);
		appWindow.titleWidgets.push_back(renameButton.get());
		appWindow.titleWidgets.push_back(deleteButton.get());
	}

	void RegionTab::onBlur() {
		renameButton.reset();
		deleteButton.reset();
	}

	void RegionTab::update() {
		if (!appWindow.game)
			return;

		auto lock = appWindow.lockGame();

		Game &game = *appWindow.game;

		auto region_ptr = game.currentRegionPointer();
		auto &region = *region_ptr;
		auto &pos = region.position;

		nameLabel.set_text(region.name);
		positionLabel.set_text("Position: " + std::string(pos));
		sizeLabel.set_text("Size: " + std::to_string(region.size));

		if (region_ptr != lastRegion) {
			lastRegion = region_ptr;

			for (Gtk::Expander &expander: expanders)
				expandersBox.remove(expander);
			expanders.clear();

			widgets.clear();

			for (const auto &pair: region.areas) {
				const std::string &area_name = pair.first;
				std::shared_ptr<Area> area = pair.second;
				Glib::ustring expander_text = Glib::Markup::escape_text(area->name);
				if (area->playerOwned)
					expander_text = "<span foreground=\"green\">" + expander_text + "</span>";
				expander_text += " (" + std::to_string(area->size) + ")";
				Gtk::Expander &expander = expanders.emplace_back(expander_text);
				expander.set_use_markup(true);
				expandersBox.append(expander);
				expander.set_margin_bottom(10);
				expander.set_expanded(true);

				auto ebox = std::make_shared<Gtk::Box>(Gtk::Orientation::VERTICAL, 5);
				widgets.push_back(ebox);
				expanderBoxes.emplace(area_name, ebox);
				expander.set_child(*ebox);

				Gtk::Box *bbox = new Gtk::Box(Gtk::Orientation::HORIZONTAL, 5);
				widgets.emplace_back(bbox);
				ebox->append(*bbox);

				Gtk::Button *button = new Gtk::Button;
				button->set_icon_name("list-add-symbolic");
				button->set_tooltip_text("Move a resource into the area");
				widgets.emplace_back(button);
				button->set_tooltip_text("Move a resource from your inventory into the area");
				bbox->append(*button);
				button->signal_clicked().connect([this, area] {
					auto *dialog = new InventoryDialog("Resource Selector", appWindow, appWindow);
					appWindow.dialog.reset(dialog);
					dialog->signal_submit().connect([this, area](const Glib::ustring &resource_name) {
						appWindow.delay([this, resource_name, area]() {
							auto *dialog = new EntryDialog<NumericEntry>("Amount", appWindow,
								"Amount of " + resource_name + " to transfer:");
							dialog->signal_submit()
							       .connect([this, resource_name, area](const Glib::ustring &amount_text) {
								double amount;
								try {
									amount = parseDouble(amount_text);
								} catch (std::invalid_argument &) {
									appWindow.delay([this] { appWindow.error("Invalid amount."); });
									return;
								}
								if (insert(area, resource_name, amount))
									update();
							});
							appWindow.dialog.reset(dialog);
							appWindow.dialog->show();
						});
					});
					appWindow.dialog->show();
				});

				button = new Gtk::Button;
				button->set_icon_name("object-flip-horizontal-symbolic");
				button->set_tooltip_text("Resize the area");
				widgets.emplace_back(button);
				bbox->append(*button);
				button->set_halign(Gtk::Align::START);
				button->signal_clicked().connect([this, area] {
					auto *dialog = new EntryDialog<NumericEntry>("Size", appWindow, "New size:");
					appWindow.dialog.reset(dialog);
					dialog->signal_submit().connect([this, area](const Glib::ustring &str) {
						size_t new_size;
						try {
							new_size = parseUlong(str);
						} catch (std::invalid_argument &) {
							appWindow.delay([this] { appWindow.error("Invalid size."); });
							return;
						}
						if (new_size < area->size) {
							if (!area->reduceSize(new_size))
								appWindow.delay([this] { appWindow.error("Couldn't resize area."); });
							else
								update();
						} else if (new_size == area->size)
							appWindow.delay([this] { appWindow.alert("That wouldn't do anything."); });
						else
							appWindow.delay([this] { appWindow.error("Increasing area size is unimplemented."); });

					});
					appWindow.dialog->show();
				});

				resourceSets[area_name].clear();
				boxMaps[area_name].clear();

				for (const auto &pair: area->resources) {
					const auto &resource_name = pair.first;
					const auto &amount = pair.second;
					resourceSets[area_name].insert(resource_name);
					decltype(Game::extractions)::iterator extraction_iter;
					Extraction *extraction = appWindow.game->getExtraction(*area, resource_name, extraction_iter);
					ebox->append(*(boxMaps[area_name][resource_name] = std::make_shared<Rbox>(appWindow, area,
						resource_name, amount, extraction, extraction_iter)));
				}
			}
		} else {
			for (auto &[area_name, area]: region.areas) {
				auto &resource_set = resourceSets[area_name];

				std::list<std::string> removed;

				for (const std::string &resource_name: resource_set)
					if (area->resources.count(resource_name) == 0)
						removed.push_back(resource_name);

				if (boxMaps.count(area_name) == 0) {
					// Resizing an area can cause a new Empty area to appear that isn't in boxMaps.
					reset();
					return;
				}

				auto &map = boxMaps.at(area_name);
				Gtk::Box &ebox = *expanderBoxes.at(area_name);

				for (const std::string &resource_name: removed) {
					ebox.remove(*map.at(resource_name));
					map.erase(resource_name);
					resource_set.erase(resource_name);
				}

				Rbox *last_rbox = nullptr;

				for (const auto &[resource_name, amount]: area->resources) {
					decltype(Game::extractions)::iterator extraction_iter;
					Extraction *extraction = appWindow.game->getExtraction(*area, resource_name, extraction_iter);
					std::shared_ptr<Rbox> rbox;
					if (resource_set.count(resource_name) == 0) {
						rbox = std::make_shared<Rbox>(appWindow, area, resource_name, amount, extraction,
							extraction_iter);
						if (!last_rbox) {
							map.emplace(resource_name, rbox);
							rbox->insert_after(ebox, *ebox.get_first_child());
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

	void RegionTab::reset() {
		lastRegion = nullptr;
		update();
	}

	bool RegionTab::insert(std::shared_ptr<Area> area, const Glib::ustring &resource_name, double amount) {
		try {
			if (amount <= 0) {
				appWindow.error("Invalid amount.");
				return false;
			}

			if (appWindow.game->inventory.count(resource_name) == 0) {
				appWindow.error("You don't have any of that resource.");
				return false;
			}

			double &in_inventory = appWindow.game->inventory.at(resource_name);
			if (ltna(in_inventory, amount)) {
				appWindow.error("You don't have enough of that resource.");
				return false;
			}

			if ((in_inventory -= amount) < Resource::MIN_AMOUNT)
				appWindow.game->inventory.erase(resource_name);

			area->resources[resource_name] += amount;
			return true;
		} catch (const std::exception &err) {
			std::cerr << "??? " << err.what() << "\n";
			appWindow.error("??? " + std::string(err.what()));
			return false;
		}
	}
}
