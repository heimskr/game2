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
		scrolled.set_child(grid);
		grid.set_row_spacing(5);
		grid.set_column_spacing(5);
		setMargins(grid, 5);
		sourceLabel.set_hexpand(true);
		destinationLabel.set_hexpand(true);
		sourceLabel.add_css_class("table-header");
		destinationLabel.add_css_class("table-header");
		resourceLabel.add_css_class("table-header");
		weightLabel.add_css_class("table-header");
		sourceLabel.set_xalign(0);
		destinationLabel.set_xalign(0);
		resourceLabel.set_xalign(0);
		weightLabel.set_xalign(0);
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
		removeChildren(grid);
		grid.attach(sourceLabel, 1, 0);
		grid.attach(destinationLabel, 2, 0);
		grid.attach(resourceLabel, 3, 0);
		grid.attach(weightLabel, 4, 0);

		if (!app.game)
			return;

		auto lock = app.lockGame();
		size_t row = 1;
		for (auto &link: app.game->automationLinks) {
			auto *button = new Gtk::Button;
			widgets.emplace_back(button);
			button->set_icon_name("list-remove-symbolic");
			button->signal_clicked().connect([this, row] {
				auto lock = app.lockGame();
				auto iter = std::next(app.game->automationLinks.begin(), row - 1);
				iter->cleanup();
				app.game->automationLinks.erase(iter);
				reset();
			});
			grid.attach(*button, 0, row);
			auto *label = new Gtk::Label(link.producer->name, Gtk::Align::START);
			widgets.emplace_back(label);
			grid.attach(*label, 1, row);
			label = new Gtk::Label(link.consumer->name, Gtk::Align::START);
			widgets.emplace_back(label);
			grid.attach(*label, 2, row);
			label = new Gtk::Label(link.resourceName, Gtk::Align::START);
			widgets.emplace_back(label);
			grid.attach(*label, 3, row);
			label = new Gtk::Label(niceDouble(link.weight), Gtk::Align::START);
			widgets.emplace_back(label);
			grid.attach(*label, 4, row);
			++row;
		}
	}
}
