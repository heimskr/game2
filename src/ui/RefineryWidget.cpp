#include <iostream>

#include "ui/RefineryWidget.h"

namespace Game2 {
	void RefineryWidget::addExtraButtons() {
		modeButton.insert_before(topBox, nameLabel);
		modeButton.set_icon_name("preferences-system-symbolic");
		modeButton.set_tooltip_text("Set refinery mode");
		modeButton.signal_clicked().connect([&] {
			std::cout << "modeButton clicked for " << processor.name << ".\n";
		});
	}
}
