#pragma once

#include "Toolkit.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "tab/Tab.h"

namespace Game2 {
	class AppWindow;
	struct CraftingRecipe;

	class CraftingTab: public Tab {
		public:
			struct Columns: public Gtk::TreeModelColumnRecord {
				Columns() {
					add(resource);
					add(amount);
					add(recipe);
				}

				Gtk::TreeModelColumn<Glib::ustring> resource;
				Gtk::TreeModelColumn<double> amount;
				Gtk::TreeModelColumn<const CraftingRecipe *> recipe;
			};

			AppWindow &appWindow;

			CraftingTab() = delete;
			CraftingTab(const CraftingTab &) = delete;
			CraftingTab(CraftingTab &&) = delete;
			CraftingTab(AppWindow &);

			CraftingTab & operator=(const CraftingTab &) = delete;
			CraftingTab & operator=(CraftingTab &&) = delete;

			Gtk::Widget & getWidget() override { return treeBox; }
			Glib::ustring getName() override { return "Crafting"; }
			void onFocus() override;
			void onBlur() override;

			void reset(bool compute_crafting = true);
			void update();

		private:
			Gtk::ScrolledWindow inputScrolled, outputScrolled;
			Gtk::Box treeBox {Gtk::Orientation::HORIZONTAL};
			Gtk::TreeView inputView, outputView;
			Glib::RefPtr<Gtk::ListStore> inputModel, outputModel;
			Columns columns;
			std::unique_ptr<Gtk::Button> addButton, helpButton;
			std::unordered_set<const CraftingRecipe *> craftingOutput;
			std::unordered_map<std::string, Gtk::TreeModel::iterator> inputRows;
			std::unordered_map<const CraftingRecipe *, Gtk::TreeModel::iterator> outputRows;
			Gtk::TreeModel::iterator addInput(const std::string &name, double amount);
			Gtk::TreeModel::iterator addOutput(const CraftingRecipe *);
			void craft(const CraftingRecipe *);
			void add();
			void remove();
			void craftRow();
			void computeCraftingOutput();
			void showHelp();
	};
}
