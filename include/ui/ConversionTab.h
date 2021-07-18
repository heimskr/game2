#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <memory>
#include <vector>

#include "ui/Tab.h"
#include "ui/ProcessorWidget.h"

namespace Game2 {
	class App;

	class ConversionTab: public Tab {
		public:
			App &app;

			ConversionTab() = delete;
			ConversionTab(const ConversionTab &) = delete;
			ConversionTab(ConversionTab &&) = delete;
			ConversionTab(App &);

			ConversionTab & operator=(const ConversionTab &) = delete;
			ConversionTab & operator=(ConversionTab &&) = delete;

			Gtk::Widget & getWidget() override { return mainBox; }
			Glib::ustring getName() override { return "Conversion"; }

			void reset();
			void update();

		private:
			Gtk::Box mainBox {Gtk::Orientation::VERTICAL};
			Gtk::Box buttonBox {Gtk::Orientation::HORIZONTAL};
			Gtk::ScrolledWindow scrolled;
			Gtk::Box vbox {Gtk::Orientation::VERTICAL};
			Gtk::Button addButton, sortButton, distributeButton;

			std::vector<std::unique_ptr<ProcessorWidget>> processorWidgets;
	};
}
