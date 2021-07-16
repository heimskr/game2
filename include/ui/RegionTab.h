#pragma once

class App;

class RegionTab {
	public:
		App &app;
		Gtk::Box &box;
		std::shared_ptr<Region> lastRegion;
		std::vector<std::unique_ptr<Gtk::Widget>> widgets;

		RegionTab() = delete;
		RegionTab(App &app_);

		void update();
		void reset();
};
