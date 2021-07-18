#include <iostream>

#include "App.h"
#include "UI.h"
#include "tab/ConversionTab.h"
#include "ui/CentrifugeWidget.h"
#include "ui/CrusherWidget.h"
#include "ui/ElectrolyzerWidget.h"
#include "ui/FermenterWidget.h"
#include "ui/FurnaceWidget.h"
#include "ui/ProcessorsDialog.h"
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
		auto *dialog = new ProcessorsDialog("Processors", *app.mainWindow, app);
		app.dialog.reset(dialog);
		dialog->signal_submit().connect([this](std::optional<Processor::Type> type) {
			if (!type.has_value())
				return;
			app.delay([this, type] {
				auto lock = app.lockGame();
				const auto &cost = app.game->processorCosts.at(*type);
				if (!contains(app.game->inventory, cost)) {
					app.error("You don't have enough resources to make that.");
					return;
				}
				for (const auto &[name, amount]: cost)
					app.game->inventory[name] -= amount;
				shrink(app.game->inventory);
				std::shared_ptr<Processor> new_processor;
				try {
					new_processor = std::shared_ptr<Processor>(Processor::ofType(*app.game, *type));
				} catch (std::invalid_argument &err) {
					app.error(err.what());
					return;
				}
				new_processor->name = Processor::typeName(*type);
				app.game->processors.push_back(new_processor);
				app.game->processorsByID.emplace(new_processor->id, new_processor);
				reset();
				app.alert("Added a new " + std::string(Processor::typeName(*type)) + ".");
			});
		});
		app.dialog->show();
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
			ProcessorWidget *widget = nullptr;
			switch (processor->getType()) {
				case Processor::Type::Centrifuge:    widget = new    CentrifugeWidget(app, *processor); break;
				case Processor::Type::Crusher:       widget = new       CrusherWidget(app, *processor); break;
				case Processor::Type::Electrolyzer:  widget = new  ElectrolyzerWidget(app, *processor); break;
				case Processor::Type::Fermenter:     widget = new     FermenterWidget(app, *processor); break;
				case Processor::Type::Furnace:       widget = new       FurnaceWidget(app, *processor); break;
				case Processor::Type::Refinery:      widget = new      RefineryWidget(app, *processor); break;
				case Processor::Type::RocketFurnace: widget = new RocketFurnaceWidget(app, *processor); break;
				default:;
			}

			if (widget) {
				processorWidgets.emplace_back(widget);
				widget->init();
				vbox.append(*widget);
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
