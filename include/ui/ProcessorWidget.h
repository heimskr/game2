#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <memory>
#include <vector>

#include "Processor.h"

class ProcessorWidget: public Gtk::Box {
	public:
		ProcessorWidget(Processor &);

		ProcessorWidget & init();
		void resetGrid();

	protected:
		Processor &processor;
		Gtk::Box topBox {Gtk::Orientation::HORIZONTAL};
		Gtk::Grid grid;
		Gtk::Button addResourceButton;
		Gtk::ToggleButton autoExtractButton;
		Gtk::Button renameButton;
		Gtk::Label nameLabel;

		virtual void addExtraButtons() = 0;

	private:
		std::vector<std::unique_ptr<Gtk::Widget>> gridWidgets;

		void addResource();
		void toggleAutoExtract();
		void rename();
};
