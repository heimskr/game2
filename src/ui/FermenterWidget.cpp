#include "App.h"
#include "ui/FermenterWidget.h"

namespace Game2 {
	void FermenterWidget::addExtraButtons() {
		fillButton.insert_before(topBox, nameLabel);
		fillButton.set_icon_name("folder-download-symbolic");
		fillButton.set_tooltip_text("Fill with fermentable items");
		fillButton.signal_clicked().connect([this] {
			for (auto &[name, amount]: app.game->inventory)
				if (app.game->resources.at(name).hasType("fermentable")) {
					processor.input[name] += amount;
					amount = 0;
				}
			shrink(app.game->inventory);
		});
	}
}
