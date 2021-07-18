#pragma once

#include <gtkmm-4.0/gtkmm.h>

#include "Processor.h"

class ProcessorWidget: public Gtk::Box {
	public:
		ProcessorWidget(Processor &);
		ProcessorWidget & init();

	protected:
		Processor &processor;
		Gtk::Box topBox {Gtk::Orientation::HORIZONTAL};
		Gtk::Button addResourceButton;
		Gtk::ToggleButton autoExtractButton;
		Gtk::Button renameButton;
		Gtk::Label nameLabel;

		virtual void addExtraButtons() = 0;

	private:
		void addResource();
		void toggleAutoExtract();
		void rename();
};
