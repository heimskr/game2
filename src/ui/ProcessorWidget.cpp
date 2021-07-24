#include "Game.h"
#include "UI.h"
#include "Util.h"
#include "ui/AppWindow.h"
#include "ui/BasicEntry.h"
#include "ui/EntryDialog.h"
#include "ui/InventoryDialog.h"
#include "ui/NumericEntry.h"
#include "ui/ProcessorWidget.h"

namespace Game2 {
	ProcessorWidget::ProcessorWidget(AppWindow &app_window, Processor &processor_):
	Gtk::Box(Gtk::Orientation::VERTICAL), appWindow(app_window), processor(processor_) {
		addResourceButton.set_tooltip_text("Add resource to processor");
		addResourceButton.set_icon_name("list-add-symbolic");
		addResourceButton.signal_clicked().connect(sigc::mem_fun(*this, &ProcessorWidget::addResource));
		topBox.append(addResourceButton);

		autoExtractButton.set_tooltip_text("Auto-extract");
		autoExtractButton.set_icon_name("emblem-synchronizing-symbolic");
		autoExtractButton.set_active(processor.autoExtract);
		autoExtractButton.signal_toggled().connect(sigc::mem_fun(*this, &ProcessorWidget::toggleAutoExtract));
		topBox.append(autoExtractButton);

		renameButton.set_tooltip_text("Rename processor");
		renameButton.set_icon_name("document-edit-symbolic");
		renameButton.signal_clicked().connect(sigc::mem_fun(*this, &ProcessorWidget::rename));
		topBox.append(renameButton);

		nameLabel.set_text(processor.name);
		topBox.append(nameLabel);

		topBox.set_spacing(5);
		append(topBox);

		inputModel = Gtk::ListStore::create(columns);
		inputView.set_model(inputModel);
		appendColumn(inputView, "Input Resource", columns.resource);
		appendColumn(inputView, "Rate", columns.amount);

		outputModel = Gtk::ListStore::create(columns);
		outputView.set_model(outputModel);
		appendColumn(outputView, "Output Resource", columns.resource);
		appendColumn(outputView, "Rate", columns.amount);

		for (auto *tree: {&inputView, &outputView})
			for (int i = 0, columns = tree->get_n_columns(); i < columns; ++i) {
				auto *column = tree->get_column(i);
				column->set_expand(true);
				column->set_resizable(true);
			}

		treeBox.set_homogeneous(true);
		treeBox.append(inputView);
		treeBox.append(outputView);
		append(treeBox);
	}

	ProcessorWidget & ProcessorWidget::init() {
		addExtraButtons();
		resetTrees();
		return *this;
	}

	void ProcessorWidget::resetTrees() {
		inputModel->clear();
		outputModel->clear();

		treeBox.set_margin_bottom(0);

		if (processor.input.empty() && processor.output.empty())
			return;

		treeBox.set_margin_bottom(20);

		inputRows.clear();
		outputRows.clear();
		inputRows.reserve(processor.input.size());
		outputRows.reserve(processor.output.size());

		for (const auto &[name, amount]: processor.input)
			insertInputRow(name, amount);

		for (const auto &[name, amount]: processor.output)
			insertOutputRow(name, amount);

		if (processor.input.empty() && processor.output.empty()) {
			inputView.hide();
			outputView.hide();
		} else {
			inputView.show();
			outputView.show();
		}
	}

	Gtk::TreeModel::iterator ProcessorWidget::insertInputRow(const std::string &resource_name, double amount) {
		auto row = inputModel->append();
		(*row)[columns.resource] = resource_name;
		(*row)[columns.amount] = amount;
		inputRows.emplace(resource_name, row);
		return row;
	}

	Gtk::TreeModel::iterator ProcessorWidget::insertOutputRow(const std::string &resource_name, double amount) {
		auto row = outputModel->append();
		(*row)[columns.resource] = resource_name;
		(*row)[columns.amount] = amount;
		outputRows.emplace(resource_name, row);
		return row;
	}

	void ProcessorWidget::updateTrees() {
		std::vector<std::string> to_remove;

		for (const auto &[resource_name, amount]: processor.input)
			if (inputRows.count(resource_name) != 0)
				(*inputRows.at(resource_name))[columns.amount] = amount;
			else
				insertInputRow(resource_name, amount);

		for (const auto &[resource_name, row]: inputRows)
			if (processor.input.count(resource_name) == 0)
				to_remove.push_back(resource_name);

		for (const std::string &resource_name: to_remove) {
			inputModel->erase(inputRows.at(resource_name));
			inputRows.erase(resource_name);
		}

		to_remove.clear();

		for (const auto &[resource_name, amount]: processor.output)
			if (outputRows.count(resource_name) != 0)
				(*outputRows.at(resource_name))[columns.amount] = amount;
			else
				insertOutputRow(resource_name, amount);

		for (const auto &[resource_name, row]: outputRows)
			if (processor.output.count(resource_name) == 0)
				to_remove.push_back(resource_name);

		for (const std::string &resource_name: to_remove) {
			outputModel->erase(outputRows.at(resource_name));
			outputRows.erase(resource_name);
		}

		if (processor.input.empty() && processor.output.empty()) {
			inputView.hide();
			outputView.hide();
		} else {
			inputView.show();
			outputView.show();
		}
	}

	void ProcessorWidget::addResource() {
		auto *dialog = new InventoryDialog("Resource Selector", appWindow, appWindow);
		appWindow.dialog.reset(dialog);
		dialog->signal_submit().connect([this](const Glib::ustring &resource_name) {
			appWindow.delay([this, resource_name]() {
				auto *dialog = new EntryDialog<NumericEntry>("Amount", appWindow,
					"Amount of " + resource_name + " to transfer:");
				dialog->signal_submit().connect([this, resource_name](const Glib::ustring &amount_text) {
					double amount;
					try {
						amount = parseDouble(amount_text);
					} catch (std::invalid_argument &) {
						appWindow.delay([this] { appWindow.error("Invalid amount."); });
						return;
					}
					if (insert(resource_name, amount))
						updateTrees();
				});
				appWindow.dialog.reset(dialog);
				appWindow.dialog->show();
			});
		});
		appWindow.dialog->show();
	}

	void ProcessorWidget::toggleAutoExtract() {
		processor.setAutoExtract(!processor.autoExtract);
		autoExtractButton.set_active(processor.autoExtract);
	}

	void ProcessorWidget::rename() {
		auto *dialog = new EntryDialog<BasicEntry>("Rename", appWindow, "New processor name:");
		appWindow.dialog.reset(dialog);
		dialog->signal_submit().connect([this](const Glib::ustring &new_name) {
			processor.setName(new_name);
			nameLabel.set_text(new_name);
		});
		appWindow.dialog->show();
	}

	bool ProcessorWidget::insert(const std::string &resource_name, double amount) {
		try {
			if (amount <= 0) {
				appWindow.error("Invalid amount.");
				return false;
			}

			if (appWindow.game->inventory.count(resource_name) == 0) {
				appWindow.error("You don't have any of that resource.");
				return false;
			}

			double &in_inventory = appWindow.game->inventory.at(resource_name);
			if (ltna(in_inventory, amount)) {
				appWindow.error("You don't have enough of that resource.");
				return false;
			}

			if ((in_inventory -= amount) < Resource::MIN_AMOUNT)
				appWindow.game->inventory.erase(resource_name);

			processor.input[resource_name] += amount;
			return true;
		} catch (const std::exception &err) {
			std::cerr << "??? " << err.what() << "\n";
			appWindow.error("??? " + std::string(err.what()));
			return false;
		}
	}
}
