#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class App;
class Region;

class RegionTab {
	public:
		App &app;
		Gtk::Box box {Gtk::Orientation::VERTICAL};
		Gtk::Label nameLabel, positionLabel, sizeLabel;
		std::shared_ptr<Region> lastRegion;
		std::vector<std::shared_ptr<Gtk::Widget>> widgets;

		std::vector<Gtk::Expander> expanders;

		// area name => box
		std::unordered_map<std::string, std::shared_ptr<Gtk::Box>> expanderBoxes;
		// area name => resource name => box
		std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Gtk::Box>>> boxMaps;
		// area name => resource set
		std::unordered_map<std::string, std::unordered_set<std::string>> resourceSets;

		RegionTab() = delete;
		RegionTab(RegionTab &&) = delete;
		RegionTab(App &app_);

		void update();
		void reset();
};