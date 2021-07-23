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
			struct Columns: public Gtk::TreeModelColumnRecord {
				Columns() {
					add(resource);
					add(amount);
				}

				Gtk::TreeModelColumn<Glib::ustring> resource;
				Gtk::TreeModelColumn<double> amount;
			};

			ProcessorWidget(App &, Processor &);

			ProcessorWidget & init();
			void resetTrees();
			void updateTrees();

			virtual void update() {}

		protected:
			App &app;
			Processor &processor;
			Gtk::Box topBox {Gtk::Orientation::HORIZONTAL};
			Gtk::Button addResourceButton;
			Gtk::ToggleButton autoExtractButton;
			Gtk::Button renameButton;
			Gtk::Label nameLabel;

			virtual void addExtraButtons() = 0;

		private:
			Gtk::Box treeBox {Gtk::Orientation::HORIZONTAL};
			Gtk::TreeView inputView, outputView;
			Glib::RefPtr<Gtk::ListStore> inputModel, outputModel;
			std::unordered_map<std::string, Gtk::TreeModel::iterator> inputRows, outputRows;
			Columns columns;

			Gtk::TreeModel::iterator insertInputRow(const std::string &resource_name, double amount);
			Gtk::TreeModel::iterator insertOutputRow(const std::string &resource_name, double amount);
			void addResource();
			void toggleAutoExtract();
			void rename();
			bool insert(const std::string &resouce_name, double amount);
	};
}
