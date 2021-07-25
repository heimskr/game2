#include <filesystem>
#include <iostream>

#include "ui/AppWindow.h"
#include "ui/UpdatingDialog.h"
#include "tab/RegionTab.h"
#include "tab/TravelTab.h"
#include "tab/InventoryTab.h"
#include "tab/ExtractionsTab.h"
#include "tab/ConversionTab.h"
#include "tab/MarketTab.h"
#include "tab/AutomationTab.h"
#include "tab/CraftingTab.h"
#include "tab/HelpTab.h"

#ifdef __linux__
#include <gtk-4.0/gdk/x11/gdkx.h>
#endif

namespace Game2 {
	AppWindow::AppWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &builder_):
		Gtk::ApplicationWindow(cobject), builder(builder_) {
		header = builder->get_widget<Gtk::HeaderBar>("headerbar");
		set_titlebar(*header);

		updateDispatcher.connect(sigc::mem_fun(*this, &AppWindow::update));

		cssProvider = Gtk::CssProvider::create();
		cssProvider->load_from_resource("/com/heimskr/game2/style.css");
		Gtk::StyleContext::add_provider_for_display(Gdk::Display::get_default(), cssProvider,
			GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

		set_child(notebook);
		set_default_size(1000, 600);
		notebook.hide();

		add_action("new", Gio::ActionMap::ActivateSlot([this] {
			resetTitle();
			activeTab->onBlur();
			notebook.show();
			auto lock = lockGame();
			game = Game::loadDefaults(*this);
			init();
		}));

		add_action("open", Gio::ActionMap::ActivateSlot([this] {
			auto lock = lockGame();
			auto *chooser = new Gtk::FileChooserDialog(*this, "Choose File", Gtk::FileChooser::Action::OPEN, true);
			dialog.reset(chooser);
			chooser->set_current_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
			chooser->set_transient_for(*this);
			chooser->set_modal(true);
			chooser->add_button("_Cancel", Gtk::ResponseType::CANCEL);
			chooser->add_button("_Open", Gtk::ResponseType::OK);
			chooser->signal_response().connect([this, chooser](int response) {
				chooser->hide();
				if (response == Gtk::ResponseType::OK)
					delay([this, chooser] {
						try {
							auto lock = lockGame();
							game = Game::load(*this, chooser->get_file()->get_path());
							resetTitle();
							activeTab->onBlur();
							init();
							notebook.show();
						} catch (std::exception &err) {
							error("Error loading save: " + std::string(err.what()));
						}
					});
			});
			chooser->signal_show().connect([this, chooser] {
				chooser->set_default_size(get_width() - 40, get_height() - 40);
				chooser->set_size_request(get_width() - 40, get_height() - 40);
				delay([chooser] {
					if (std::filesystem::exists("save.txt"))
						chooser->set_file(Gio::File::create_for_path("save.txt"));
				});
			});
			chooser->show();
		}));

		     activeTab = regionTab = std::make_shared<RegionTab>(*this);
		     travelTab = std::make_shared<TravelTab>(*this);
		  inventoryTab = std::make_shared<InventoryTab>(*this);
		extractionsTab = std::make_shared<ExtractionsTab>(*this);
		 conversionTab = std::make_shared<ConversionTab>(*this);
		     marketTab = std::make_shared<MarketTab>(*this);
		 automationTab = std::make_shared<AutomationTab>(*this);
		   craftingTab = std::make_shared<CraftingTab>(*this);
		       helpTab = std::make_shared<HelpTab>(*this);

		addTab(regionTab);
		addTab(travelTab);
		addTab(inventoryTab);
		addTab(extractionsTab);
		addTab(conversionTab);
		addTab(marketTab);
		addTab(automationTab);
		addTab(craftingTab);
		addTab(helpTab);

		notebook.signal_unmap().connect([this] {
			notebookConnection.disconnect();
		});

		notebook.signal_show().connect([this] {
			notebookConnection = notebook.signal_switch_page().connect([this](Gtk::Widget *, guint page_number) {
				resetTitle();
				activeTab->onBlur();
				activeTab = tabs.at(page_number);
				activeTab->onFocus();
			});
		});

		moneyDispatcher.connect([this] {
			marketTab->updateMoney();
		});
	}

	AppWindow::~AppWindow() {
		alive = false;
		tickThread.join();
	}

	AppWindow * AppWindow::create() {
		auto builder = Gtk::Builder::create_from_resource("/com/heimskr/game2/window.ui");
		auto window = Gtk::Builder::get_widget_derived<AppWindow>(builder, "app_window");
		if (!window)
			throw std::runtime_error("No \"app_window\" object in window.ui");
		return window;
	}

	void AppWindow::init() {
		onTravel();
		     regionTab->reset();
		     travelTab->reset();
		  inventoryTab->update();
		extractionsTab->reset();
		 conversionTab->reset();
		     marketTab->reset();
		 automationTab->reset();
		   craftingTab->reset();
		     activeTab->onFocus();
		connectSave();
	}

	void AppWindow::update() {
		if (dialog)
			if (auto *udialog = dynamic_cast<UpdatingDialog *>(dialog.get()))
				udialog->updateData();

		    travelTab->reset();
		 inventoryTab->update();
		    regionTab->update();
		conversionTab->update();
		    marketTab->update();
	}

	void AppWindow::resetTitle() {
		for (auto &child: titleWidgets)
			header->remove(*child);
		titleWidgets.clear();
	}

	void AppWindow::delay(std::function<void()> fn) {
		add_tick_callback([fn](const auto &) {
			fn();
			return false;
		});
	}

	void AppWindow::alert(const Glib::ustring &message, Gtk::MessageType type, bool modal, bool use_markup) {
		dialog.reset(new Gtk::MessageDialog(*this, message, use_markup, type, Gtk::ButtonsType::OK, modal));
		dialog->signal_response().connect([this](int) {
			dialog->close();
		});
		dialog->show();
	}

	void AppWindow::error(const Glib::ustring &message, bool modal, bool use_markup) {
		alert(message, Gtk::MessageType::ERROR, modal, use_markup);
	}

	void AppWindow::onTravel() {
		     regionTab->update();
		     travelTab->reset();
		extractionsTab->reset();
		     marketTab->reset();
	}

	void AppWindow::hackWindow() {
#ifdef __linux__
		if (get_width() == 0 && get_height() == 0) {
			delay(sigc::mem_fun(*this, &AppWindow::hackWindow));
		} else {
			Display *display = gdk_x11_display_get_xdisplay(gdk_display_get_default());
			auto window = gdk_x11_surface_get_xid(get_surface()->gobj());
			XWindowAttributes attrs;
			int status = XGetWindowAttributes(display, window, &attrs);
			if (status == 0) {
				std::cerr << "XGetWindowAttributes failed\n";
			} else {
				const int screen = DefaultScreen(display);
				const int window_width = attrs.width;
				const int window_height = attrs.height;
				const int display_width = DisplayWidth(display, screen);
				const int display_height = DisplayHeight(display, screen);
				XMoveWindow(display, window, (display_width - window_width) / 2, (display_height - window_height) / 2);
			}
		}
#endif
	}

	void AppWindow::connectSave() {
		add_action("save", Gio::ActionMap::ActivateSlot([this] {
			auto lock = lockGame();
			if (!game)
				return;
			if (game->path.empty()) {
				choosePath(sigc::mem_fun(*this, &AppWindow::saveGame));
			} else {
				auto lock = lockGame();
				game->save();
			}
		}));

		add_action("save_as", Gio::ActionMap::ActivateSlot([this] {
			auto lock = lockGame();
			if (!game)
				return;
			choosePath(sigc::mem_fun(*this, &AppWindow::saveGame));
		}));
	}

	void AppWindow::saveGame() {
		auto lock = lockGame();
		try {
			game->save();
		} catch (std::exception &err) {
			const std::string what = err.what();
			delay([this, what] { error("Couldn't save game: " + what); });
		}
	}

	void AppWindow::choosePath(std::function<void()> after) {
		auto *chooser = new Gtk::FileChooserDialog(*this, "Save Location", Gtk::FileChooser::Action::SAVE, true);
		dialog.reset(chooser);
		chooser->set_current_folder(Gio::File::create_for_path(std::filesystem::current_path().string()));
		chooser->set_transient_for(*this);
		chooser->set_modal(true);
		chooser->add_button("_Cancel", Gtk::ResponseType::CANCEL);
		chooser->add_button("_Save", Gtk::ResponseType::OK);
		chooser->signal_response().connect([this, chooser, after](int response) {
			chooser->hide();
			if (response == Gtk::ResponseType::OK) {
				auto lock = lockGame();
				game->path = chooser->get_file()->get_path();
				after();
			}
		});
		chooser->signal_show().connect([this, chooser] {
			chooser->set_default_size(get_width() - 40, get_height() - 40);
			chooser->set_size_request(get_width() - 40, get_height() - 40);
		});
		chooser->show();
	}

	void AppWindow::addTab(std::shared_ptr<Tab> tab) {
		notebook.append_page(tab->getWidget(), tab->getName());
		tabs.push_back(tab);
	}
}
