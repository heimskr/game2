#include <cassert>
#include <iostream>

#include "Util.h"
#include "processor/RocketFurnace.h"
#include "ui/RocketFurnaceWidget.h"

namespace Game2 {
	void RocketFurnaceWidget::addExtraButtons() {
		fillButton.insert_before(topBox, nameLabel);
		fillButton.set_icon_name("folder-download-symbolic");
		fillButton.set_tooltip_text("Fill with smeltable items");
		fillButton.signal_clicked().connect([&] {
			std::cout << "fillButton clicked for " << processor.name << ".\n";
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