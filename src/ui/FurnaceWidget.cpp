#include <cassert>

#include "Game.h"
#include "processor/Furnace.h"
#include "ui/AppWindow.h"
#include "ui/FurnaceWidget.h"

namespace Game2 {
	void FurnaceWidget::addExtraButtons() {
		fillButton.insert_before(topBox, nameLabel);
		fillButton.set_icon_name("folder-download-symbolic");
		fillButton.set_tooltip_text("Fill with smeltable items");
		fillButton.signal_clicked().connect([this] {
			for (auto &[name, amount]: appWindow.game->inventory)
				if (appWindow.game->resources.at(name).hasType("smeltable")) {
					processor.input[name] += amount;
					amount = 0;
				}
			shrink(appWindow.game->inventory);
		});
		topBox.append(fuelLabel);
	}

	void FurnaceWidget::update() {
		Furnace *furnace = dynamic_cast<Furnace *>(&processor);
		assert(furnace);
		if (furnace->fuel != lastFuel) {
			lastFuel = furnace->fuel;
			fuelLabel.set_text("(" + niceDouble(furnace->fuel) + " fuel)");
		}
	}
}
