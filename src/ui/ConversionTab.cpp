#include "App.h"
#include "UI.h"
#include "ui/ConversionTab.h"
#include "ui/CentrifugeWidget.h"

ConversionTab::ConversionTab(App &app_): app(app_) {
	addButton.set_icon_name("list-add-symbolic");
	addButton.set_tooltip_text("Add processor");
	sortButton.set_icon_name("view-sort-descending-symbolic");
	sortButton.set_tooltip_text("Sort processors");
	distributeButton.set_icon_name("emblem-shared-symbolic");
	distributeButton.set_tooltip_text("Distribute resource among all processors of a given type");
	buttonBox.append(addButton);
	buttonBox.append(sortButton);
	buttonBox.append(distributeButton);
	buttonBox.set_spacing(5);
	mainBox.append(buttonBox);
	vbox.set_spacing(5);
	mainBox.append(vbox);
	mainBox.set_spacing(5);
	setMargins(mainBox, 5);
	reset();
}

void ConversionTab::reset() {
	removeChildren(vbox);
	widgets.clear();

	auto lock = app.lockGame();
	if (!app.game)
		return;

	for (auto &processor: app.game->processors) {
		switch (processor->getType()) {
			case Processor::Type::Centrifuge: {
				auto *widget = new CentrifugeWidget(*processor);
				widgets.emplace_back(widget);
				widget->init();
				vbox.append(*widget);
				break;
			}

			default:;
		}
	}
}
