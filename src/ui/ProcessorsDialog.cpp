#include "Game.h"
#include "UI.h"

#include "ui/AppWindow.h"
#include "ui/ProcessorsDialog.h"

namespace Game2 {
	ProcessorsDialog::ProcessorsDialog(const Glib::ustring &title, Gtk::Window &parent, AppWindow &app_window,
	                                   bool modal):
	Dialog(title, parent, modal), appWindow(app_window) {
		set_default_size(300, -1);

		scrolled.set_size_request(-1, 300);
		scrolled.set_hexpand(true);
		scrolled.set_vexpand(true);
		scrolled.set_child(treeView);

		treeModel = Gtk::ListStore::create(columns);
		treeView.set_model(treeModel);
		treeView.signal_row_activated().connect(sigc::mem_fun(*this, &ProcessorsDialog::rowActivated));

		auto *column = appendColumn(treeView, "Name", columns.name);
		column->set_expand(true);
		column->set_resizable(true);
		column = appendColumn(treeView, "Type", columns.type);
		column->set_resizable(true);

		signal_hide().connect([this] {
			if (!done) {
				done = true;
				signal_submit_.emit(nullptr);
			}
		});

		signal_close_request().connect([this] {
			signal_submit_.emit(nullptr);
			return false;
		}, true);

		reset();

		get_content_area()->append(scrolled);
	}

	void ProcessorsDialog::reset() {
		auto lock = appWindow.lockGame();
		for (auto &processor: appWindow.game->processors) {
			auto row = *treeModel->append();
			row[columns.name] = processor->name;
			row[columns.type] = Processor::typeName(processor->getType());
			row[columns.processor] = processor;
		}
	}

	void ProcessorsDialog::rowActivated(const Gtk::TreeModel::Path &, Gtk::TreeViewColumn *) {
		if (auto iter = treeView.get_selection()->get_selected()) {
			hide();
			signal_submit_.emit(iter->get_value(columns.processor));
		}
	}
}
