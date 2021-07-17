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
		std::vector<std::unique_ptr<Gtk::Widget>> widgets;

		std::unordered_map<std::string, Gtk::Box *> expanderBoxes; // area => box
		std::unordered_map<std::string, std::unordered_map<std::string, Gtk::Box *>> boxMaps; // area => resource => box
		std::unordered_map<std::string, std::unordered_set<std::string>> resourceSets; // area => resources

		RegionTab() = delete;
		RegionTab(RegionTab &&) = delete;
		RegionTab(App &app_);

		void update();
		void reset();
};
