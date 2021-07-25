#include <iostream>

#include "Game.h"
#include "UI.h"
#include "tab/ConversionTab.h"
#include "ui/AppWindow.h"
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
	ConversionTab::ConversionTab(AppWindow &app_window): appWindow(app_window) {
		vbox.set_spacing(5);
		setMargins(vbox, 5);
		scrolled.set_child(vbox);
		scrolled.set_vexpand(true);
		reset();
	}

	void ConversionTab::add() {
		auto *dialog = new ProcessorTypeDialog("Processors", appWindow, appWindow);
		appWindow.dialog.reset(dialog);
		dialog->signal_submit().connect([this](std::optional<Processor::Type> type) {
			if (!type.has_value())
				return;
			appWindow.delay([this, type] {
				auto lock = appWindow.lockGame();
				const auto &cost = appWindow.game->processorCosts.at(*type);
				if (!contains(appWindow.game->inventory, cost)) {
					appWindow.error("You don't have enough resources to make that.");
					return;
				}
				for (const auto &[name, amount]: cost)
					appWindow.game->inventory[name] -= amount;
				shrink(appWindow.game->inventory);
				std::shared_ptr<Processor> new_processor;
				try {
					new_processor = std::shared_ptr<Processor>(Processor::ofType(*appWindow.game, *type));
				} catch (std::invalid_argument &err) {
					appWindow.error(err.what());
					return;
				}
				new_processor->name = Processor::typeName(*type);
				appWindow.game->processors.push_back(new_processor);
				appWindow.game->processorsByID.emplace(new_processor->id, new_processor);
				reset();
				appWindow.alert("Added a new " + std::string(Processor::typeName(*type)) + ".");
			});
		});
		appWindow.dialog->show();
	}

	void ConversionTab::sort() {
		auto lock = appWindow.lockGame();
		appWindow.game->processors.sort([](std::shared_ptr<Processor> &left, std::shared_ptr<Processor> &right) {
			return left->name < right->name;
		});
		reset();
	}

	void ConversionTab::distribute() {
		auto *dialog = new InventoryDialog("Resource Selector", appWindow, appWindow);
		appWindow.dialog.reset(dialog);
		dialog->signal_submit().connect([this](const Glib::ustring &resource_name) {
			appWindow.delay([this, resource_name]() {
				auto *dialog = new EntryDialog<NumericEntry>("Amount", appWindow,
					"Amount of " + resource_name + " to distribute:");
				dialog->signal_submit().connect([this, resource_name](const Glib::ustring &amount_text) {
					appWindow.delay([this, resource_name, amount_text] {
						appWindow.gameMutex.lock();
						if (appWindow.game->inventory.count(resource_name) == 0) {
							appWindow.gameMutex.unlock();
							appWindow.error("You don't have any of that resource.");
							return;
						}

						double amount, &in_inventory = appWindow.game->inventory.at(resource_name);
						if (amount_text.empty())
							amount = in_inventory;
						else
							try {
								amount = parseDouble(amount_text);
							} catch (std::invalid_argument &) {
								appWindow.gameMutex.unlock();
								appWindow.error("Invalid amount.");
								return;
							}

						if (lte(amount, 0) || ltna(in_inventory, amount)) {
							appWindow.gameMutex.unlock();
							appWindow.error("You don't have enough of that resource.");
							return;
						}

						auto *dialog = new ProcessorTypeDialog("Processors", appWindow, appWindow);
						appWindow.dialog.reset(dialog);
						dialog->signal_submit().connect([this, resource_name, amount, &in_inventory](auto type) {
							if (!type.has_value()) {
								appWindow.gameMutex.unlock();
								return;
							}

							appWindow.delay([this, resource_name, amount, &in_inventory, type] {
								size_t count = 0;
								for (const auto &processor: appWindow.game->processors)
									if (processor->getType() == *type)
										++count;

								if (count == 0) {
									appWindow.gameMutex.unlock();
									appWindow.error("You don't have any " + std::string(Processor::typeName(*type))
										+ " processors.");
									return;
								}

								for (auto &processor: appWindow.game->processors)
									if (processor->getType() == *type)
										processor->input[resource_name] += amount / count;
								in_inventory -= amount;
								shrink(appWindow.game->inventory, resource_name);
								appWindow.gameMutex.unlock();
							});
						});
						appWindow.dialog->show();
					});
				});
				appWindow.dialog.reset(dialog);
				appWindow.dialog->show();
			});
		});
		appWindow.dialog->show();
	}

	void ConversionTab::toggleAutomation() {
		if (!appWindow.game)
			return;

		auto lock = appWindow.lockGame();
		appWindow.game->automationEnabled = !appWindow.game->automationEnabled;
		updateAutomationButton();
	}

	void ConversionTab::onFocus() {
		addButton = std::make_unique<Gtk::Button>();
		addButton->set_icon_name("list-add-symbolic");
		addButton->set_tooltip_text("Add processor");
		addButton->signal_clicked().connect(sigc::mem_fun(*this, &ConversionTab::add));
		appWindow.header->pack_start(*addButton);
		appWindow.titleWidgets.push_back(addButton.get());

		sortButton = std::make_unique<Gtk::Button>();
		sortButton->set_icon_name("view-sort-descending-symbolic");
		sortButton->set_tooltip_text("Sort processors");
		sortButton->signal_clicked().connect(sigc::mem_fun(*this, &ConversionTab::sort));
		appWindow.header->pack_start(*sortButton);
		appWindow.titleWidgets.push_back(sortButton.get());

		distributeButton = std::make_unique<Gtk::Button>();
		distributeButton->set_icon_name("emblem-shared-symbolic");
		distributeButton->set_tooltip_text("Distribute resource among all processors of a given type");
		distributeButton->signal_clicked().connect(sigc::mem_fun(*this, &ConversionTab::distribute));
		appWindow.header->pack_start(*distributeButton);
		appWindow.titleWidgets.push_back(distributeButton.get());

		automationButton = std::make_unique<Gtk::Button>();
		automationButton->signal_clicked().connect(sigc::mem_fun(*this, &ConversionTab::toggleAutomation));
		appWindow.header->pack_start(*automationButton);
		appWindow.titleWidgets.push_back(automationButton.get());
		updateAutomationButton();
	}

	void ConversionTab::onBlur() {
		addButton.reset();
		sortButton.reset();
		distributeButton.reset();
		automationButton.reset();
	}

	void ConversionTab::updateAutomationButton() {
		if (!appWindow.game || !automationButton)
			return;

		auto lock = appWindow.lockGame();

		if (appWindow.game->automationEnabled) {
			automationButton->set_icon_name("media-playback-pause-symbolic");
			automationButton->set_tooltip_text("Pause automation");
		} else {
			automationButton->set_icon_name("media-playback-start-symbolic");
			automationButton->set_tooltip_text("Start automation");
		}
	}

	void ConversionTab::reset() {
		removeChildren(vbox);
		processorWidgets.clear();

		auto lock = appWindow.lockGame();
		if (!appWindow.game)
			return;

		for (auto &processor: appWindow.game->processors) {
			ProcessorWidget *widget = nullptr;
			switch (processor->getType()) {
				case Processor::Type::Centrifuge:    widget = new    CentrifugeWidget(appWindow, *processor); break;
				case Processor::Type::Crusher:       widget = new       CrusherWidget(appWindow, *processor); break;
				case Processor::Type::Electrolyzer:  widget = new  ElectrolyzerWidget(appWindow, *processor); break;
				case Processor::Type::Fermenter:     widget = new     FermenterWidget(appWindow, *processor); break;
				case Processor::Type::Furnace:       widget = new       FurnaceWidget(appWindow, *processor); break;
				case Processor::Type::Refinery:      widget = new      RefineryWidget(appWindow, *processor); break;
				case Processor::Type::RocketFurnace: widget = new RocketFurnaceWidget(appWindow, *processor); break;
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
