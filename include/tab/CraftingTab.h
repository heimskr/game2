#pragma once

#include <gtkmm-4.0/gtkmm.h>
#include <memory>
#include <vector>

#include "tab/Tab.h"

namespace Game2 {
	class App;
	class CraftingRecipe;

	class CraftingTab: public Tab {
		public:
			App &app;

			CraftingTab() = delete;
			CraftingTab(const CraftingTab &) = delete;
			CraftingTab(CraftingTab &&) = delete;
			CraftingTab(App &);

			CraftingTab & operator=(const CraftingTab &) = delete;
			CraftingTab & operator=(CraftingTab &&) = delete;

			Gtk::Widget & getWidget() override { return scrolled; }
			Glib::ustring getName() override { return "Crafting"; }
			void onFocus() override;
			void onBlur() override;

			void reset(bool compute_crafting = true);

		private:
			Gtk::ScrolledWindow scrolled;
			Gtk::Box gridBox {Gtk::Orientation::HORIZONTAL};
			Gtk::Grid inputGrid, outputGrid;
			Gtk::Separator separator;
			Gtk::Label inputLabel {"Input"}, inputAmountLabel {"Amount"}, outputLabel {"Output"},
			           outputAmountLabel {"Amount"};
			std::unique_ptr<Gtk::Button> addButton, helpButton;
			std::vector<std::unique_ptr<Gtk::Widget>> widgets;
			std::vector<const CraftingRecipe *> craftingOutput;
			void craft(const CraftingRecipe *);
			void add();
			void computeCraftingOutput();
			void showHelp();
	};
}