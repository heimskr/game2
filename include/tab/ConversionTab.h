#pragma once

#include "Toolkit.h"
#include <memory>
#include <vector>

#include "tab/Tab.h"
#include "ui/ProcessorWidget.h"

namespace Game2 {
	class AppWindow;

	class ConversionTab: public Tab {
		public:
			AppWindow &appWindow;

			ConversionTab() = delete;
			ConversionTab(const ConversionTab &) = delete;
			ConversionTab(ConversionTab &&) = delete;
			ConversionTab(AppWindow &);

			ConversionTab & operator=(const ConversionTab &) = delete;
			ConversionTab & operator=(ConversionTab &&) = delete;

			Gtk::Widget & getWidget() override { return scrolled; }
			Glib::ustring getName() override { return "Conversion"; }
			void onFocus() override;
			void onBlur() override;

			void updateAutomationButton();
			void reset();
			void update();

		private:
			Gtk::Box mainBox {Gtk::Orientation::VERTICAL};
			Gtk::Box buttonBox {Gtk::Orientation::HORIZONTAL};
			Gtk::ScrolledWindow scrolled;
			Gtk::Box vbox {Gtk::Orientation::VERTICAL};
			std::unique_ptr<Gtk::Button> addButton, sortButton, distributeButton, automationButton;
			std::vector<std::unique_ptr<ProcessorWidget>> processorWidgets;

			void add();
			void sort();
			void distribute();
			void toggleAutomation();
	};
}
