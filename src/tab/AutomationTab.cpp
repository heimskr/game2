#include <iostream>

#include "UI.h"
#include "tab/AutomationTab.h"
#include "ui/AppWindow.h"
#include "ui/EntryDialog.h"
#include "ui/NumericEntry.h"
#include "ui/ProcessorsDialog.h"
#include "ui/ProcessorTypeDialog.h"
#include "ui/ResourcesDialog.h"

namespace Game2 {
	AutomationTab::AutomationTab(AppWindow &app_window): appWindow(app_window) {
		scrolled.set_vexpand(true);
		scrolled.set_child(treeView);
		treeModel = Gtk::ListStore::create(columns);
		treeView.set_model(treeModel);
		appendColumn(treeView, "Source", columns.source);
		appendColumn(treeView, "Destination", columns.destination);
		appendColumn(treeView, "Resource", columns.resource);
		appendColumn(treeView, "Weight", columns.weight);
		for (int i = 0, columns = treeView.get_n_columns(); i < columns; ++i) {
			auto *column = treeView.get_column(i);
			column->set_expand(true);
			column->set_resizable(true);
		}
		treeView.signal_row_activated().connect(sigc::mem_fun(*this, &AutomationTab::rowActivated));
		reset();
	}
	
	void AutomationTab::onFocus() {
		if (!appWindow.game)
			return;

		addButton = std::make_unique<Gtk::Button>();
		addButton->set_icon_name("list-add-symbolic");
		addButton->signal_clicked().connect(sigc::mem_fun(*this, &AutomationTab::addLink));
		appWindow.header->pack_start(*addButton);
		appWindow.titleWidgets.push_back(addButton.get());

		removeButton = std::make_unique<Gtk::Button>();
		removeButton->set_icon_name("list-remove-symbolic");
		removeButton->signal_clicked().connect(sigc::mem_fun(*this, &AutomationTab::removeLink));
		appWindow.header->pack_start(*removeButton);
		appWindow.titleWidgets.push_back(removeButton.get());
	}

	void AutomationTab::onBlur() {
		addButton.reset();
		removeButton.reset();
	}

	void AutomationTab::reset() {
		treeModel->clear();

		if (!appWindow.game)
			return;

		auto lock = appWindow.lockGame();
		auto &links = appWindow.game->automationLinks;

		for (auto iter = links.begin(), end = links.end(); iter != end; ++iter) {
			AutomationLink &link = *iter;
			auto row = *treeModel->append();
			row[columns.source] = link.producer->name;
			row[columns.destination] = link.consumer->name;
			row[columns.resource] = link.resourceName;
			row[columns.weight] = niceDouble(link.weight);
			row[columns.iter] = iter;
		}
	}

	void AutomationTab::rowActivated(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column) {
		auto iter = treeModel->get_iter(path);
		switch (column->get_sort_column_id()) {
			case 0: { // Source
				auto *dialog = new ProcessorsDialog("Source Processor", appWindow, appWindow);
				appWindow.dialog.reset(dialog);
				dialog->signal_submit().connect([this, iter](std::shared_ptr<Processor> source) {
					if (!source)
						return;
					appWindow.dialog->hide();
					auto lock = appWindow.lockGame();
					iter->set_value(columns.source, Glib::ustring(source->name));
					iter->get_value(columns.iter)->producer = source;
				});
				dialog->show();
				break;
			}
			case 1: // Destination

				break;
			case 2: { // Resource
				auto *dialog = new ResourcesDialog("Resource", appWindow, appWindow);
				appWindow.dialog.reset(dialog);
				dialog->signal_submit().connect([this, iter](const std::string &rtype) {
					auto lock = appWindow.lockGame();
					iter->set_value(columns.resource, Glib::ustring(rtype));
					iter->get_value(columns.iter)->resourceName = rtype;
				});
				dialog->show();
				break;
			}
			case 3: // Weight

				break;
		}
	}

	void AutomationTab::addLink() {
		auto *dialog = new ProcessorsDialog("Source Processor", appWindow, appWindow);
		appWindow.dialog.reset(dialog);
		dialog->signal_submit().connect([this](std::shared_ptr<Processor> src) {
			if (!src)
				return;
			appWindow.delay([this, src] {
				auto *dialog = new ResourcesDialog("Resource", appWindow, appWindow);
				appWindow.dialog.reset(dialog);
				dialog->signal_submit().connect([this, src](const std::string &rtype) {
					if (rtype.empty())
						return;
					// Oh god I'm in callback hell
					appWindow.delay([this, src, rtype] {
						auto *dialog = new ProcessorsDialog("Destination Processor", appWindow, appWindow);
						appWindow.dialog.reset(dialog);
						dialog->signal_submit().connect([this, src, rtype](std::shared_ptr<Processor> dest) {
							if (!dest)
								return;
							appWindow.delay([this, src, rtype, dest] {
								auto *dialog = new EntryDialog<NumericEntry>("Weight", appWindow, "Weight:");
								appWindow.dialog.reset(dialog);
								dialog->signal_submit().connect([this, src, rtype, dest](const Glib::ustring &str) {
									double weight;
									try {
										if (lte(weight = parseDouble(str), 0))
											throw std::invalid_argument("Invalid weight.");
									} catch (std::invalid_argument &) {
										appWindow.delay([this] { appWindow.error("Invalid weight."); });
										return;
									}
									auto lock = appWindow.lockGame();
									appWindow.game->automationLinks.emplace_back(*appWindow.game, src, dest, rtype,
										weight).setup();
									appWindow.delay([this, src, rtype, dest] {
										appWindow.alert("Linked " + src->name + " to " + dest->name + " for " + rtype
											+ ".");
									});
									reset();
								});
								appWindow.dialog->show();
							});
						});
						appWindow.dialog->show();
					});
				});
				appWindow.dialog->show();
			});
		});
		appWindow.dialog->show();
	}

	void AutomationTab::removeLink() {
		if (auto iter = treeView.get_selection()->get_selected()) {
			auto link_iter = iter->get_value(columns.iter);
			link_iter->cleanup();
			appWindow.game->automationLinks.erase(link_iter);
			treeModel->erase(iter);
		}
	}
}
