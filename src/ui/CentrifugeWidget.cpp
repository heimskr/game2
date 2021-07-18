#include "App.h"
#include "ui/CentrifugeWidget.h"

namespace Game2 {
	void CentrifugeWidget::addExtraButtons() {
		fillButton.insert_before(topBox, nameLabel);
		fillButton.set_icon_name("folder-download-symbolic");
		fillButton.set_tooltip_text("Fill with centrifugable items");
		fillButton.signal_clicked().connect([this] {
			for (auto &[name, amount]: app.game->inventory)
				if (app.game->resources.at(name).hasType("centrifugable")) {
					processor.input[name] += amount;
					amount = 0;
				}
			shrink(app.game->inventory);
		});
	}
}
