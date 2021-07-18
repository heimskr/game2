#include "App.h"
#include "UI.h"
#include "tab/ConversionTab.h"
#include "ui/CentrifugeWidget.h"
#include "ui/CrusherWidget.h"
#include "ui/ElectrolyzerWidget.h"
#include "ui/FermenterWidget.h"
#include "ui/FurnaceWidget.h"
#include "ui/RefineryWidget.h"
#include "ui/RocketFurnaceWidget.h"

namespace Game2 {
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
		scrolled.set_child(vbox);
		scrolled.set_vexpand(true);
		mainBox.append(scrolled);
		mainBox.set_spacing(5);
		setMargins(mainBox, 5);
		reset();
	}

	void ConversionTab::reset() {
		removeChildren(vbox);
		processorWidgets.clear();

		auto lock = app.lockGame();
		if (!app.game)
			return;

		for (auto &processor: app.game->processors) {
			switch (processor->getType()) {
				case Processor::Type::Centrifuge: {
					auto *widget = new CentrifugeWidget(app, *processor);
					processorWidgets.emplace_back(widget);
					widget->init();
					vbox.append(*widget);
					break;
				}

				case Processor::Type::Crusher: {
					auto *widget = new CrusherWidget(app, *processor);
					processorWidgets.emplace_back(widget);
					widget->init();
					vbox.append(*widget);
					break;
				}

				case Processor::Type::Electrolyzer: {
					auto *widget = new ElectrolyzerWidget(app, *processor);
					processorWidgets.emplace_back(widget);
					widget->init();
					vbox.append(*widget);
					break;
				}

				case Processor::Type::Fermenter: {
					auto *widget = new FermenterWidget(app, *processor);
					processorWidgets.emplace_back(widget);
					widget->init();
					vbox.append(*widget);
					break;
				}

				case Processor::Type::Furnace: {
					auto *widget = new FurnaceWidget(app, *processor);
					processorWidgets.emplace_back(widget);
					widget->init();
					vbox.append(*widget);
					break;
				}

				case Processor::Type::Refinery: {
					auto *widget = new RefineryWidget(app, *processor);
					processorWidgets.emplace_back(widget);
					widget->init();
					vbox.append(*widget);
					break;
				}

				case Processor::Type::RocketFurnace: {
					auto *widget = new RocketFurnaceWidget(app, *processor);
					processorWidgets.emplace_back(widget);
					widget->init();
					vbox.append(*widget);
					break;
				}

				default:;
			}
		}
	}

	void ConversionTab::update() {
		for (auto &widget: processorWidgets) {
			widget->updateGrid();
			widget->update();
		}
	}
}
