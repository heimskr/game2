#pragma once

#include "Toolkit.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "tab/Tab.h"

namespace Game2 {
	class App;
	class CraftingRecipe;

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
			void update();

		private:
			Gtk::ScrolledWindow scrolled;
			Gtk::Box treeBox {Gtk::Orientation::HORIZONTAL};
			Gtk::TreeView inputView, outputView;
			Glib::RefPtr<Gtk::ListStore> inputModel, outputModel;
			Columns columns;
			std::unique_ptr<Gtk::Button> addButton, removeButton, craftButton, helpButton;
			std::unordered_set<const CraftingRecipe *> craftingOutput;
			std::unordered_map<std::string, Gtk::TreeModel::iterator> inputRows;
			std::unordered_map<const CraftingRecipe *, Gtk::TreeModel::iterator> outputRows;
			Gtk::TreeModel::iterator addInput(const std::string &name, double amount);
			Gtk::TreeModel::iterator addOutput(const CraftingRecipe *);
			void craft(const CraftingRecipe *);
			void add();
			void remove();
			void craftClicked();
			void computeCraftingOutput();
			void showHelp();
	};
}
