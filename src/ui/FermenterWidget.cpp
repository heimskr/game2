#include <cassert>

#include "Game.h"
#include "processor/Fermenter.h"
#include "ui/AppWindow.h"
#include "ui/FermenterWidget.h"

namespace Game2 {
	void FermenterWidget::addExtraButtons() {
		fillButton.insert_before(topBox, nameLabel);
		fillButton.set_icon_name("folder-download-symbolic");
		fillButton.set_tooltip_text("Fill with fermentable items");
		fillButton.signal_clicked().connect([this] {
			for (auto &[name, amount]: appWindow.game->inventory)
				if (appWindow.game->resources.at(name).hasType("fermentable")) {
					processor.input[name] += amount;
					amount = 0;
				}
			shrink(appWindow.game->inventory);
		});
		topBox.append(yeastLabel);
	}

	void FermenterWidget::update() {
		Fermenter *fermenter = dynamic_cast<Fermenter *>(&processor);
		assert(fermenter);
		if (fermenter->yeast != lastYeast) {
			lastYeast = fermenter->yeast;
			yeastLabel.set_text("(" + niceDouble(fermenter->yeast) + " yeast)");
		}
	}
}
