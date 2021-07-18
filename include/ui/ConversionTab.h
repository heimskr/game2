#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <memory>
#include <vector>

#include "ui/Tab.h"

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

	private:
		Gtk::Box mainBox {Gtk::Orientation::VERTICAL};
		Gtk::Box buttonBox {Gtk::Orientation::HORIZONTAL};
		Gtk::ScrolledWindow scrolled;
		Gtk::Box vbox {Gtk::Orientation::VERTICAL};
		Gtk::Button addButton, sortButton, distributeButton;

		std::vector<std::unique_ptr<Gtk::Widget>> widgets;
};