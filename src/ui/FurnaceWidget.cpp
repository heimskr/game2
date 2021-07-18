#include "App.h"
#include "ui/FurnaceWidget.h"

namespace Game2 {
	void FurnaceWidget::addExtraButtons() {
		fillButton.insert_before(topBox, nameLabel);
		fillButton.set_icon_name("folder-download-symbolic");
		fillButton.set_tooltip_text("Fill with smeltable items");
		fillButton.signal_clicked().connect([this] {
			for (auto &[name, amount]: app.game->inventory)
				if (app.game->resources.at(name).hasType("smeltable")) {
					processor.input[name] += amount;
					amount = 0;
				}
			shrink(app.game->inventory);
		});
	}
}
