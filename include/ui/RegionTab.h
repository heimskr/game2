#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ui/Tab.h"

class App;
class Region;

class RegionTab: public Tab {
	public:
		struct Rbox: public Gtk::Box {
			Gtk::Button extractButton {"Extract"};
			Gtk::Label label;
			std::function<void()> onClick;
			Rbox() = delete;
			Rbox(const std::string &resource_name, double amount, std::function<void()> on_click);
			void updateLabel(const std::string &resource_name, double amount);
		};

		App &app;
		Gtk::ScrolledWindow scrolled;
		Gtk::Box box {Gtk::Orientation::VERTICAL}, expandersBox {Gtk::Orientation::VERTICAL};
		Gtk::Box buttonBox {Gtk::Orientation::HORIZONTAL}, labelBox {Gtk::Orientation::VERTICAL};
		Gtk::Button renameButton, deleteButton;
		Gtk::Label nameLabel, positionLabel, sizeLabel;
		std::shared_ptr<Region> lastRegion;
		std::vector<std::shared_ptr<Gtk::Widget>> widgets;

		std::vector<Gtk::Expander> expanders;

		// area name => ebox
		std::unordered_map<std::string, std::shared_ptr<Gtk::Box>> expanderBoxes;
		// area name => resource name => rbox
		std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Rbox>>> boxMaps;
		// area name => resource set
		std::unordered_map<std::string, std::unordered_set<std::string>> resourceSets;

		RegionTab() = delete;
		RegionTab(const RegionTab &) = delete;
		RegionTab(RegionTab &&) = delete;
		RegionTab(App &);

		RegionTab & operator=(const RegionTab &) = delete;
		RegionTab & operator=(RegionTab &&) = delete;

		Gtk::Widget & getWidget() override { return box; }
		Glib::ustring getName() override { return "Region"; }

		void update();
		void reset();

	private:
		bool insert(std::shared_ptr<Area>, const Glib::ustring &resource_name, double amount);
};
