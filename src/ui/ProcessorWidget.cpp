#include <iostream>

#include "UI.h"
#include "ui/EntryDialog.h"
#include "ui/InventoryDialog.h"
#include "ui/NumericEntry.h"
#include "ui/ProcessorWidget.h"

ProcessorWidget::ProcessorWidget(App &app_, Processor &processor_):
Gtk::Box(Gtk::Orientation::VERTICAL), app(app_), processor(processor_) {
	addResourceButton.set_tooltip_text("Add resource to processor");
	addResourceButton.set_icon_name("list-add-symbolic");
	addResourceButton.signal_clicked().connect(sigc::mem_fun(*this, &ProcessorWidget::addResource));
	topBox.append(addResourceButton);

	autoExtractButton.set_tooltip_text("Auto-extract");
	autoExtractButton.set_icon_name("emblem-synchronizing-symbolic");
	autoExtractButton.set_active(processor.autoExtract);
	autoExtractButton.signal_toggled().connect(sigc::mem_fun(*this, &ProcessorWidget::toggleAutoExtract));
	topBox.append(autoExtractButton);

	renameButton.set_tooltip_text("Rename processor");
	renameButton.set_icon_name("document-edit-symbolic");
	renameButton.signal_clicked().connect(sigc::mem_fun(*this, &ProcessorWidget::rename));
	topBox.append(renameButton);

	nameLabel.set_text(processor.name);
	topBox.append(nameLabel);

	topBox.set_spacing(5);
	append(topBox);

	grid.set_row_spacing(5);
	grid.set_column_spacing(5);
	grid.set_column_homogeneous(true);
	append(grid);
}

ProcessorWidget & ProcessorWidget::init() {
	addExtraButtons();
	resetGrid();
	return *this;
}

void ProcessorWidget::resetGrid() {
	removeChildren(grid);
	gridWidgets.clear();

	if (processor.input.empty() && processor.output.empty())
		return;

	auto *label = new Gtk::Label("Input Resource", Gtk::Align::START);
	gridWidgets.emplace_back(label);
	grid.attach(*label, 0, 0);

	label = new Gtk::Label("Amount", Gtk::Align::START);
	gridWidgets.emplace_back(label);
	grid.attach(*label, 1, 0);

	label = new Gtk::Label("Output Resource", Gtk::Align::START);
	gridWidgets.emplace_back(label);
	grid.attach(*label, 2, 0);

	label = new Gtk::Label("Amount", Gtk::Align::START);
	gridWidgets.emplace_back(label);
	grid.attach(*label, 3, 0);

	int row = 1;
	for (const auto &[name, amount]: processor.input) {
		label = new Gtk::Label(name, Gtk::Align::START);
		gridWidgets.emplace_back(label);
		grid.attach(*label, 0, row);

		label = new Gtk::Label(std::to_string(amount), Gtk::Align::START);
		gridWidgets.emplace_back(label);
		grid.attach(*label, 1, row);

		++row;
	}

	row = 1;

	for (const auto &[name, amount]: processor.output) {
		label = new Gtk::Label(name, Gtk::Align::START);
		gridWidgets.emplace_back(label);
		grid.attach(*label, 2, row);

		label = new Gtk::Label(std::to_string(amount), Gtk::Align::START);
		gridWidgets.emplace_back(label);
		grid.attach(*label, 3, row);

		++row;
	}
}

void ProcessorWidget::addResource() {
	auto *dialog = new InventoryDialog("Resource Selector", *app.mainWindow);
	app.dialog.reset(dialog);
	dialog->signal_submit().connect([this](const Glib::ustring &resource_name) {
		app.delay([this, resource_name]() {
			auto *dialog = new EntryDialog<NumericEntry>("Amount", *app.mainWindow,
				"Amount of " + resource_name + " to transfer:");
			dialog->signal_submit().connect([this, resource_name](const Glib::ustring &amount_text) {
				double amount;
				try {
					amount = parseDouble(amount_text);
				} catch (std::invalid_argument &) {
					app.delay([this] { app.error("Invalid amount."); });
					return;
				}
				if (insert(resource_name, amount))
					resetGrid();
			});
			app.dialog.reset(dialog);
			app.dialog->show();
		});
	});
	app.dialog->show();
}

void ProcessorWidget::toggleAutoExtract() {
	std::cout << "toggleAutoExtract(" << processor.name << ")\n";
}

void ProcessorWidget::rename() {
	std::cout << "rename(" << processor.name << ")\n";
}

bool ProcessorWidget::insert(const std::string &resource_name, double amount) {
	try {
		if (amount <= 0) {
			app.error("Invalid amount.");
			return false;
		}

		if (app.game->inventory.count(resource_name) == 0) {
			app.error("You don't have any of that resource.");
			return false;
		}

		double &in_inventory = app.game->inventory.at(resource_name);
		if (ltna(in_inventory, amount)) {
			app.error("You don't have enough of that resource.");
			return false;
		}

		if ((in_inventory -= amount) < Resource::MIN_AMOUNT)
			app.game->inventory.erase(resource_name);

		processor.input[resource_name] += amount;
		return true;
	} catch (const std::exception &err) {
		std::cerr << "??? " << err.what() << "\n";
		app.error("??? " + std::string(err.what()));
		return false;
	}
}
