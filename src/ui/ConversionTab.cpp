#include "App.h"
#include "UI.h"
#include "ui/ConversionTab.h"

ConversionTab::ConversionTab(App &app_): app(app_) {
	reset();
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
	mainBox.append(vbox);
	mainBox.set_spacing(5);
	setMargins(mainBox, 5);
}

void ConversionTab::reset() {
	
}
