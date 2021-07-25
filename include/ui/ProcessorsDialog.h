#pragma once

#include "Toolkit.h"
#include <memory>
#include <vector>

namespace Game2 {
	class AppWindow;
	class Processor;

	class ProcessorsDialog: public Gtk::Dialog {
		public:
			struct Columns: public Gtk::TreeModelColumnRecord {
				Columns() {
					add(name);
					add(type);
					add(processor);
				}

				Gtk::TreeModelColumn<Glib::ustring> name, type;
				Gtk::TreeModelColumn<std::shared_ptr<Processor>> processor;
			};

			ProcessorsDialog(const Glib::ustring &title, Gtk::Window &parent, AppWindow &, bool modal = true);

			sigc::signal<void(std::shared_ptr<Processor>)> signal_submit() const { return signal_submit_; }
			void reset();

		private:
			AppWindow &appWindow;
			bool done = false;
			sigc::signal<void(std::shared_ptr<Processor>)> signal_submit_;
			Gtk::ScrolledWindow scrolled;
			Gtk::TreeView treeView;
			Glib::RefPtr<Gtk::ListStore> treeModel;
			Columns columns;

			void rowActivated(const Gtk::TreeModel::Path &, Gtk::TreeViewColumn *);
	};
}
