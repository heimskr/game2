#pragma once

#include "Toolkit.h"
#include <list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "tab/Tab.h"

namespace Game2 {
	class App;
	class Area;
	class Extraction;
	class Region;

	class RegionTab: public Tab {
		public:
			class Rbox: public Gtk::Box {
				public:
					Rbox() = delete;
					Rbox(App &, std::shared_ptr<Area>, const std::string &resource_name, double amount, Extraction *,
					     std::list<Extraction>::iterator);
					void updateLabel(const std::string &resource_name, double amount);

				private:
					App &app;
					std::shared_ptr<Area> area;
					Gtk::Button extractButton {"Extract"};
					Gtk::Box labelBox {Gtk::Orientation::HORIZONTAL, 5};
					Gtk::Label mainLabel, extractionLabel;
					Extraction *extraction;
					std::list<Extraction>::iterator extractionIter;
					Glib::RefPtr<Gtk::GestureClick> extractionGesture;
			};

			App &app;
			Gtk::ScrolledWindow scrolled;
			Gtk::Box box {Gtk::Orientation::VERTICAL}, expandersBox {Gtk::Orientation::VERTICAL};
			Gtk::Box labelBox {Gtk::Orientation::VERTICAL};
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
			void onFocus() override;
			void onBlur() override;

			void update();
			void reset();

		private:
			std::unique_ptr<Gtk::Button> renameButton, deleteButton;
			bool insert(std::shared_ptr<Area>, const Glib::ustring &resource_name, double amount);
	};
}
