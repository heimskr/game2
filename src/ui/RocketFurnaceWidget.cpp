#include <cassert>

#include "Game.h"
#include "Util.h"
#include "processor/RocketFurnace.h"
#include "ui/AppWindow.h"
#include "ui/RocketFurnaceWidget.h"

namespace Game2 {
	void RocketFurnaceWidget::addExtraButtons() {
		fillButton.insert_before(topBox, nameLabel);
		fillButton.set_icon_name("folder-download-symbolic");
		fillButton.set_tooltip_text("Fill with smeltable items");
		fillButton.signal_clicked().connect([this] {
			if (appWindow.game->inventory.count("Hydrogen") != 0 && appWindow.game->inventory.count("Oxygen") != 0) {
				// Ensure the ratio of fuel added is always 2 Hydrogen per 1 Oxygen.
				double oxygen = appWindow.game->inventory.at("Oxygen");
				double hydrogen = std::min(appWindow.game->inventory.at("Hydrogen"), oxygen * 2);
				oxygen = std::min(oxygen, hydrogen / 2.);
				processor.input["Hydrogen"] += hydrogen;
				appWindow.game->inventory.at("Hydrogen") -= hydrogen;
				processor.input["Oxygen"] += oxygen;
				appWindow.game->inventory.at("Oxygen") -= oxygen;
				for (auto &[name, amount]: appWindow.game->inventory) {
					const Resource &resource = appWindow.game->resources.at(name);
					if (resource.hasType("rocket smeltable")) {
						processor.input[name] += amount;
						amount = 0;
					}
				}
			} else {
				for (auto &[name, amount]: appWindow.game->inventory) {
					const Resource &resource = appWindow.game->resources.at(name);
					if (name == "Hydrogen" || name == "Oxygen" || resource.hasType("rocket smeltable")) {
						processor.input[name] += amount;
						amount = 0;
					}
				}
			}
			shrink(appWindow.game->inventory);
		});
		topBox.append(fuelLabel);
		update();
	}

	void RocketFurnaceWidget::update() {
		RocketFurnace *furnace = dynamic_cast<RocketFurnace *>(&processor);
		assert(furnace);
		fuelLabel.set_text("(" + niceDouble(furnace->hydrogen) + " H, " + niceDouble(furnace->oxygen) + " O)");
	}
}
