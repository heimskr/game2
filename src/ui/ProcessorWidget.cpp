#include <iostream>

#include "UI.h"
#include "ui/ProcessorWidget.h"

ProcessorWidget::ProcessorWidget(Processor &processor_): Gtk::Box(Gtk::Orientation::VERTICAL), processor(processor_) {
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
	std::cout << "addResource(" << processor.name << ")\n";
}

void ProcessorWidget::toggleAutoExtract() {
	std::cout << "toggleAutoExtract(" << processor.name << ")\n";
}

void ProcessorWidget::rename() {
	std::cout << "rename(" << processor.name << ")\n";
}
