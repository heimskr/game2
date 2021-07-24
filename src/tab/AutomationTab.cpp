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

	AutomationTab::AutomationTab(AppWindow &window_): window(window_) {
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
		reset();
	}
	
	void AutomationTab::onFocus() {
		if (!window.game)
			return;

		addButton = std::make_unique<Gtk::Button>();
		addButton->set_icon_name("list-add-symbolic");
		addButton->signal_clicked().connect(sigc::mem_fun(*this, &AutomationTab::addLink));
		window.header->pack_start(*addButton);
		window.titleWidgets.push_back(addButton.get());

		removeButton = std::make_unique<Gtk::Button>();
		removeButton->set_icon_name("list-remove-symbolic");
		removeButton->signal_clicked().connect(sigc::mem_fun(*this, &AutomationTab::removeLink));
		window.header->pack_start(*removeButton);
		window.titleWidgets.push_back(removeButton.get());
	}

	void AutomationTab::onBlur() {
		addButton.reset();
		removeButton.reset();
	}

	void AutomationTab::reset() {
		treeModel->clear();

		if (!window.game)
			return;

		auto lock = window.lockGame();
		auto &links = window.game->automationLinks;

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

	void AutomationTab::addLink() {
		auto *dialog = new ProcessorsDialog("Source Processor", window, window);
		window.dialog.reset(dialog);
		dialog->signal_submit().connect([this](std::shared_ptr<Processor> src) {
			if (!src)
				return;
			window.delay([this, src] {
				auto *dialog = new ResourcesDialog("Resource", window, window);
				window.dialog.reset(dialog);
				dialog->signal_submit().connect([this, src](std::string rtype) {
					if (rtype.empty())
						return;
					// Oh god I'm in callback hell
					window.delay([this, src, rtype] {
						auto *dialog = new ProcessorsDialog("Destination Processor", window, window);
						window.dialog.reset(dialog);
						dialog->signal_submit().connect([this, src, rtype](std::shared_ptr<Processor> dest) {
							if (!dest)
								return;
							window.delay([this, src, rtype, dest] {
								auto *dialog = new EntryDialog<NumericEntry>("Weight", window, "Weight:");
								window.dialog.reset(dialog);
								dialog->signal_submit().connect([this, src, rtype, dest](const Glib::ustring &str) {
									double weight;
									try {
										if (lte(weight = parseDouble(str), 0))
											throw std::invalid_argument("Invalid weight.");
									} catch (std::invalid_argument &) {
										window.delay([this] { window.error("Invalid weight."); });
										return;
									}
									auto lock = window.lockGame();
									window.game->automationLinks.emplace_back(*window.game, src, dest, rtype,
										weight).setup();
									window.delay([this, src, rtype, dest] {
										window.alert("Linked " + src->name + " to " + dest->name + " for " + rtype
											+ ".");
									});
									reset();
								});
								window.dialog->show();
							});
						});
						window.dialog->show();
					});
				});
				window.dialog->show();
			});
		});
		window.dialog->show();
	}

	void AutomationTab::removeLink() {
		if (auto iter = treeView.get_selection()->get_selected()) {
			auto link_iter = iter->get_value(columns.iter);
			link_iter->cleanup();
			window.game->automationLinks.erase(link_iter);
			treeModel->erase(iter);
		}
	}
}
