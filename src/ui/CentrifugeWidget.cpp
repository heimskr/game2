#include <iostream>

#include "ui/CentrifugeWidget.h"

void CentrifugeWidget::addExtraButtons() {
	fillButton.insert_before(topBox, nameLabel);
	fillButton.set_icon_name("folder-download-symbolic");
	fillButton.set_tooltip_text("Fill with centrifugable items");
	fillButton.signal_clicked().connect([&] {
		std::cout << "fillButton clicked for " << processor.name << ".\n";
	});
}