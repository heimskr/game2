#pragma once

#include "Toolkit.h"
#include <memory>
#include <vector>

#include "Game.h"
#include "tab/Tab.h"

namespace Game2 {
	class AppWindow;

	class ExtractionsTab: public Tab {
		public:
			struct Columns: public Gtk::TreeModelColumnRecord {
				Columns() {
					add(resource);
					add(area);
					add(region);
					add(rate);
					add(iter);
				}

				Gtk::TreeModelColumn<Glib::ustring> resource, area, region, rate;
				Gtk::TreeModelColumn<decltype(Game::extractions)::iterator> iter;
			};

			AppWindow &appWindow;

			ExtractionsTab() = delete;
			ExtractionsTab(const ExtractionsTab &) = delete;
			ExtractionsTab(ExtractionsTab &&) = delete;
			ExtractionsTab(AppWindow &);

			ExtractionsTab & operator=(const ExtractionsTab &) = delete;
			ExtractionsTab & operator=(ExtractionsTab &&) = delete;

			Gtk::Widget & getWidget() override { return scrolled; }
			Glib::ustring getName() override { return "Extractions"; }
			void onFocus() override;
			void onBlur() override;

			void reset();

		private:
			bool global = false;
			Gtk::ScrolledWindow scrolled;
			std::unique_ptr<Gtk::ToggleButton> globalButton;
			std::unique_ptr<Gtk::Button> removeButton;
			Gtk::TreeView treeView;
			Glib::RefPtr<Gtk::ListStore> treeModel;
			Columns columns;

			void toggleGlobal();
			void removeExtraction();
	};
}
