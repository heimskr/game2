#include <iostream>

#include "App.h"
#include "UI.h"
#include "tab/ConversionTab.h"
#include "ui/CentrifugeWidget.h"
#include "ui/CrusherWidget.h"
#include "ui/ElectrolyzerWidget.h"
#include "ui/EntryDialog.h"
#include "ui/FermenterWidget.h"
#include "ui/FurnaceWidget.h"
#include "ui/InventoryDialog.h"
#include "ui/NumericEntry.h"
#include "ui/ProcessorTypeDialog.h"
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
		auto *dialog = new ProcessorTypeDialog("Processors", *app.mainWindow, app);
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
		auto lock = app.lockGame();
		app.game->processors.sort([](std::shared_ptr<Processor> &left, std::shared_ptr<Processor> &right) {
			return left->name < right->name;
		});
		reset();
	}

	void ConversionTab::distribute() {
		auto *dialog = new InventoryDialog("Resource Selector", *app.mainWindow);
		app.dialog.reset(dialog);
		dialog->signal_submit().connect([this](const Glib::ustring &resource_name) {
			app.delay([this, resource_name]() {
				auto *dialog = new EntryDialog<NumericEntry>("Amount", *app.mainWindow,
					"Amount of " + resource_name + " to distribute:");
				dialog->signal_submit().connect([this, resource_name](const Glib::ustring &amount_text) {
					app.delay([this, resource_name, amount_text] {
						double amount;
						try {
							amount = parseDouble(amount_text);
						} catch (std::invalid_argument &) {
							app.error("Invalid amount.");
							return;
						}
						app.gameMutex.lock();
						if (app.game->inventory.count(resource_name) == 0) {
							app.gameMutex.unlock();
							app.error("You don't have any of that resource.");
							return;
						}
						double &in_inventory = app.game->inventory.at(resource_name);
						if (lte(amount, 0) || ltna(in_inventory, amount)) {
							app.gameMutex.unlock();
							app.error("You don't have enough of that resource.");
							return;
						}
						auto *dialog = new ProcessorTypeDialog("Procesors", *app.mainWindow, app);
						app.dialog.reset(dialog);
						dialog->signal_submit().connect([this, resource_name, amount, &in_inventory](auto type) {
							if (!type.has_value()) {
								app.gameMutex.unlock();
								return;
							}

							app.delay([this, resource_name, amount, &in_inventory, type] {
								size_t count = 0;
								for (const auto &processor: app.game->processors)
									if (processor->getType() == *type)
										++count;

								if (count == 0) {
									app.gameMutex.unlock();
									app.error("You don't have any " + std::string(Processor::typeName(*type))
										+ " processors.");
									return;
								}

								for (auto &processor: app.game->processors)
									if (processor->getType() == *type)
										processor->input[resource_name] += amount / count;
								in_inventory -= amount;
								shrink(app.game->inventory);
								app.gameMutex.unlock();
							});
						});
						app.dialog->show();
					});
				});
				app.dialog.reset(dialog);
				app.dialog->show();
			});
		});
		app.dialog->show();
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
			widget->updateTrees();
			widget->update();
		}
	}
}
