#include "Game.h"
#include "ui/AppWindow.h"
#include "ui/CrusherWidget.h"

namespace Game2 {
	void CrusherWidget::addExtraButtons() {
		fillButton.insert_before(topBox, nameLabel);
		fillButton.set_icon_name("folder-download-symbolic");
		fillButton.set_tooltip_text("Fill with crushable items");
		fillButton.signal_clicked().connect([this] {
			auto lock = appWindow.lockGame();
			for (auto &[name, amount]: appWindow.game->inventory)
				if (appWindow.game->resources.at(name).hasType("crushable")) {
					processor.input[name] += amount;
					amount = 0;
				}
			shrink(appWindow.game->inventory);
		});
	}
}
