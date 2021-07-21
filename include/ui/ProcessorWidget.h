#pragma once

#include "Toolkit.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Processor.h"

namespace Game2 {
	class App;

	class ProcessorWidget: public Gtk::Box {
		public:
			ProcessorWidget(App &, Processor &);

			ProcessorWidget & init();
			void resetGrid();
			void updateGrid();

			virtual void update() {}

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
			std::unordered_map<std::string, Gtk::Label> inputNames, inputAmounts, outputNames, outputAmounts;
			std::unordered_set<std::string> previousInputs, previousOutputs;

			void addResource();
			void toggleAutoExtract();
			void rename();
			bool insert(const std::string &resouce_name, double amount);
	};
}
