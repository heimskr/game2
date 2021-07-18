#include <iostream>

#include "ui/FurnaceWidget.h"

void FurnaceWidget::addExtraButtons() {
	fillButton.insert_before(topBox, nameLabel);
	fillButton.set_icon_name("folder-download-symbolic");
	fillButton.set_tooltip_text("Fill with smeltable items");
	fillButton.signal_clicked().connect([&] {
		std::cout << "fillButton clicked for " << processor.name << ".\n";
	});
}
