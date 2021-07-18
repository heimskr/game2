#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <memory>
#include <vector>

#include "ui/Tab.h"

class App;

class ExtractionsTab: public Tab {
	public:
		App &app;

		ExtractionsTab() = delete;
		ExtractionsTab(const ExtractionsTab &) = delete;
		ExtractionsTab(ExtractionsTab &&) = delete;
		ExtractionsTab(App &);

		ExtractionsTab & operator=(const ExtractionsTab &) = delete;
		ExtractionsTab & operator=(ExtractionsTab &&) = delete;

		void reset();

	private:
		Gtk::ScrolledWindow scrolled;
		Gtk::Grid grid;

		std::vector<std::unique_ptr<Gtk::Widget>> widgets;

		Gtk::Widget & getWidget() override { return scrolled; }
		Glib::ustring getName() override { return "Extractions"; }
};

