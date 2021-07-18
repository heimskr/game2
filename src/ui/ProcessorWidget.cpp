#include <iostream>

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
}

ProcessorWidget & ProcessorWidget::init() {
	addExtraButtons();
	return *this;
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
