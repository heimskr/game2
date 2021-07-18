#include <iostream>

#include "UI.h"
#include "ui/BasicEntry.h"
#include "ui/EntryDialog.h"
#include "ui/InventoryDialog.h"
#include "ui/NumericEntry.h"
#include "ui/ProcessorWidget.h"

static bool compare(const std::unordered_set<std::string> &set, const Resource::Map &map) {
	if (set.size() != map.size())
		return false;

	for (const auto &[resource_name, amount]: map)
		if (!set.contains(resource_name))
			return false;
	
	return true;
}

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
	inputNames.clear();
	inputAmounts.clear();
	outputNames.clear();
	outputAmounts.clear();
	grid.set_margin_bottom(0);

	if (processor.input.empty() && processor.output.empty())
		return;

	grid.set_margin_bottom(20);

	{
		auto *label = new Gtk::Label("Input Resource", Gtk::Align::START);
		label->add_css_class("table-header");
		gridWidgets.emplace_back(label);
		grid.attach(*label, 0, 0);

		label = new Gtk::Label("Amount", Gtk::Align::START);
		label->add_css_class("table-header");
		gridWidgets.emplace_back(label);
		grid.attach(*label, 1, 0);

		label = new Gtk::Label("Output Resource", Gtk::Align::START);
		label->add_css_class("table-header");
		gridWidgets.emplace_back(label);
		grid.attach(*label, 2, 0);

		label = new Gtk::Label("Amount", Gtk::Align::START);
		label->add_css_class("table-header");
		gridWidgets.emplace_back(label);
		grid.attach(*label, 3, 0);
	}

	previousInputs.clear();
	previousOutputs.clear();
	previousInputs.reserve(processor.input.size());
	previousOutputs.reserve(processor.output.size());
	int row = 1;

	for (const auto &[name, amount]: processor.input) {
		previousInputs.insert(name);

		auto &nlabel = inputNames.emplace(name, Gtk::Label(name, Gtk::Align::START)).first->second;
		grid.attach(nlabel, 0, row);

		auto &alabel = inputAmounts.emplace(name, Gtk::Label(std::to_string(amount), Gtk::Align::START)).first->second;
		grid.attach(alabel, 1, row);

		++row;
	}

	row = 1;

	for (const auto &[name, amount]: processor.output) {
		previousOutputs.insert(name);

		auto &nlabel = outputNames.emplace(name, Gtk::Label(name, Gtk::Align::START)).first->second;
		grid.attach(nlabel, 2, row);

		auto &alabel = outputAmounts.emplace(name, Gtk::Label(std::to_string(amount), Gtk::Align::START)).first->second;
		grid.attach(alabel, 3, row);

		++row;
	}
}

void ProcessorWidget::updateGrid() {
	if (!compare(previousInputs, processor.input) || !compare(previousOutputs, processor.output)) {
		resetGrid();
	} else {
		for (const auto &[resource_name, amount]: processor.input) {
			const Glib::ustring amount_string = std::to_string(amount);
			Gtk::Label &label = inputAmounts.at(resource_name);
			if (label.get_text() != amount_string)
				label.set_text(amount_string);
		}

		for (const auto &[resource_name, amount]: processor.output) {
			const Glib::ustring amount_string = std::to_string(amount);
			Gtk::Label &label = outputAmounts.at(resource_name);
			if (label.get_text() != amount_string)
				label.set_text(amount_string);
		}
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
					updateGrid();
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
	auto *dialog = new EntryDialog<BasicEntry>("Rename", *app.mainWindow, "New processor name:");
	app.dialog.reset(dialog);
	dialog->signal_submit().connect([this](const Glib::ustring &new_name) {
		processor.setName(new_name);
		nameLabel.set_text(new_name);
	});
	app.dialog->show();
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
