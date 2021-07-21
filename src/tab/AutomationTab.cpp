#include <iostream>

#include "App.h"
#include "UI.h"
#include "tab/AutomationTab.h"
#include "ui/EntryDialog.h"
#include "ui/NumericEntry.h"
#include "ui/ProcessorsDialog.h"
#include "ui/ProcessorTypeDialog.h"
#include "ui/ResourcesDialog.h"

namespace Game2 {

	AutomationTab::AutomationTab(App &app_): app(app_) {
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
		if (!app.game)
			return;

		addButton = std::make_unique<Gtk::Button>();
		addButton->set_icon_name("list-add-symbolic");
		addButton->signal_clicked().connect([this] {
			auto *dialog = new ProcessorsDialog("Source Processor", *app.mainWindow, app);
			app.dialog.reset(dialog);
			dialog->signal_submit().connect([this](std::shared_ptr<Processor> src) {
				if (!src)
					return;
				app.delay([this, src] {
					auto *dialog = new ResourcesDialog("Resource", *app.mainWindow, app);
					app.dialog.reset(dialog);
					dialog->signal_submit().connect([this, src](std::string rtype) {
						if (rtype.empty())
							return;
						// Oh god I'm in callback hell
						app.delay([this, src, rtype] {
							auto *dialog = new ProcessorsDialog("Destination Processor", *app.mainWindow, app);
							app.dialog.reset(dialog);
							dialog->signal_submit().connect([this, src, rtype](std::shared_ptr<Processor> dest) {
								if (!dest)
									return;
								app.delay([this, src, rtype, dest] {
									auto *dialog = new EntryDialog<NumericEntry>("Weight", *app.mainWindow, "Weight:");
									app.dialog.reset(dialog);
									dialog->signal_submit().connect([this, src, rtype, dest](const Glib::ustring &str) {
										double weight;
										try {
											if (lte(weight = parseDouble(str), 0))
												throw std::invalid_argument("Invalid weight.");
										} catch (std::invalid_argument &) {
											app.delay([this] { app.error("Invalid weight."); });
											return;
										}
										auto lock = app.lockGame();
										app.game->automationLinks.emplace_back(*app.game, src, dest, rtype, weight)
										                         .setup();
										app.delay([this, src, rtype, dest] {
											app.alert("Linked " + src->name + " to " + dest->name + " for " + rtype
												+ ".");
										});
										reset();
									});
									app.dialog->show();
								});
							});
							app.dialog->show();
						});
					});
					app.dialog->show();
				});
			});
			app.dialog->show();
		});

		app.header->pack_start(*addButton);
		app.titleWidgets.push_back(addButton.get());
	}

	void AutomationTab::onBlur() {
		addButton.reset();
	}

	void AutomationTab::reset() {
		treeModel->clear();

		if (!app.game)
			return;

		auto lock = app.lockGame();

		for (auto &link: app.game->automationLinks) {
			auto row = *treeModel->append();
			row[columns.source] = link.producer->name;
			row[columns.destination] = link.consumer->name;
			row[columns.resource] = link.resourceName;
			row[columns.weight] = niceDouble(link.weight);
		}
	}
}
