#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <memory>
#include <vector>

#include "Processor.h"

class App;

class ProcessorWidget: public Gtk::Box {
	public:
		ProcessorWidget(App &, Processor &);

		ProcessorWidget & init();
		void resetGrid();

	protected:
		App &app;
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
		std::vector<std::string> previousInputs, previousOutputs;

		void addResource();
		void toggleAutoExtract();
		void rename();
		bool insert(const std::string &resouce_name, double amount);
};
