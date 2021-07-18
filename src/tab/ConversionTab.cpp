#include <iostream>

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
		vbox.set_spacing(5);
		setMargins(vbox, 5);
		scrolled.set_child(vbox);
		scrolled.set_vexpand(true);
		reset();
	}

	void ConversionTab::add() {
		std::cout << "add\n";
	}

	void ConversionTab::sort() {
		std::cout << "sort\n";
	}

	void ConversionTab::distribute() {
		std::cout << "distribute\n";
	}

	void ConversionTab::onBlur() {
		addButton.reset();
		sortButton.reset();
		distributeButton.reset();
	}

	void ConversionTab::onFocus() {
		addButton = std::make_unique<Gtk::Button>();
		addButton->set_icon_name("list-add-symbolic");
		addButton->set_tooltip_text("Add processor");
		addButton->signal_clicked().connect(sigc::mem_fun(*this, &ConversionTab::add));
		sortButton = std::make_unique<Gtk::Button>();
		sortButton->set_icon_name("view-sort-descending-symbolic");
		sortButton->set_tooltip_text("Sort processors");
		sortButton->signal_clicked().connect(sigc::mem_fun(*this, &ConversionTab::sort));
		distributeButton = std::make_unique<Gtk::Button>();
		distributeButton->set_icon_name("emblem-shared-symbolic");
		distributeButton->set_tooltip_text("Distribute resource among all processors of a given type");
		distributeButton->signal_clicked().connect(sigc::mem_fun(*this, &ConversionTab::distribute));
		app.header->pack_start(*addButton);
		app.header->pack_start(*sortButton);
		app.header->pack_start(*distributeButton);
		app.titleWidgets.push_back(addButton.get());
		app.titleWidgets.push_back(sortButton.get());
		app.titleWidgets.push_back(distributeButton.get());
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
