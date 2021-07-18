#include <iostream>

#include "App.h"
#include "UI.h"
#include "ui/InventoryDialog.h"

#include <gtk-4.0/gdk/x11/gdkx.h>

namespace Game2 {
	std::unique_ptr<App> app;

	App::App(Glib::RefPtr<Gtk::Application> gtk_app): gtkApp(gtk_app) {
		updateDispatcher.connect(sigc::mem_fun(*this, &App::update));

		mainWindow = std::make_unique<Gtk::ApplicationWindow>();
		builder = Gtk::Builder::create();
		builder->add_from_file("main.ui");

		saveButton = builder->get_widget<Gtk::Button>("save_button");
		saveButton->set_visible(false);

		cssProvider = Gtk::CssProvider::create();
		cssProvider->load_from_path("style.css");
		Gtk::StyleContext::add_provider_for_display(Gdk::Display::get_default(), cssProvider,
			GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

		header = builder->get_widget<Gtk::HeaderBar>("headerbar");
		mainWindow->set_titlebar(*header);

		notebook = std::make_unique<Gtk::Notebook>();
		mainWindow->set_child(*notebook);
		mainWindow->set_default_size(1000, 600);
		notebook->hide();

		mainWindow->add_action("new", Gio::ActionMap::ActivateSlot([this] {
			notebook->show();
			auto lock = lockGame();
			game = Game::loadDefaults(*this);
			saveButton->set_visible(true);
			onTravel();
			inventoryTab->reset();
			conversionTab->reset();
			marketTab->reset();
			connectSave();
		}));

		mainWindow->add_action("open", Gio::ActionMap::ActivateSlot([this] {
			notebook->show();
			auto lock = lockGame();
			game = Game::load(*this);
			saveButton->set_visible(true);
			onTravel();
			inventoryTab->reset();
			conversionTab->reset();
			marketTab->reset();
			connectSave();
		}));

		// mainWindow->add_action("save_as", Gio::ActionMap::ActivateSlot([&] {}));

		regionTab = std::make_unique<RegionTab>(*this);
		travelTab = std::make_unique<TravelTab>(*this);
		inventoryTab = std::make_unique<InventoryTab>(*this);
		extractionsTab = std::make_unique<ExtractionsTab>(*this);
		conversionTab = std::make_unique<ConversionTab>(*this);
		marketTab = std::make_unique<MarketTab>(*this);

		addTab(*regionTab);
		addTab(*travelTab);
		addTab(*inventoryTab);
		addTab(*extractionsTab);
		addTab(*conversionTab);
		addTab(*marketTab);

		moneyDispatcher.connect([this] {
			marketTab->updateMoney();
		});
	}

	void App::connectSave() {
		mainWindow->add_action("save", Gio::ActionMap::ActivateSlot([this] {
			auto lock = lockGame();
			if (game)
				game->save();
		}));
	}

	void App::addTab(Tab &tab) {
	notebook->append_page(tab.getWidget(), tab.getName());
	}

	void App::quit() {
		gtkApp->quit();
	}

	void App::delay(std::function<void()> fn) {
		mainWindow->add_tick_callback([fn](const auto &) {
			fn();
			return false;
		});
	}

	void App::alert(const Glib::ustring &message, Gtk::MessageType type, bool modal, bool use_markup) {
		dialog.reset(new Gtk::MessageDialog(*mainWindow, message, use_markup, type, Gtk::ButtonsType::OK, modal));
		dialog->signal_response().connect([this](int) {
			dialog->close();
		});
		dialog->show();
	}

	void App::error(const Glib::ustring &message, bool modal, bool use_markup) {
		alert(message, Gtk::MessageType::ERROR, modal, use_markup);
	}

	int App::run(int argc, char **argv) {
		gtkApp->signal_activate().connect([this] {
			gtkApp->add_window(*mainWindow);

			mainWindow->signal_show().connect([this] {
				delay(sigc::mem_fun(*this, &App::hackWindow));
			});

			mainWindow->show();
		});
		return gtkApp->run(argc, argv);
	}

	void App::onTravel() {
		regionTab->update();
		travelTab->update();
		extractionsTab->reset();
	}

	void App::update() {
		if (dialog)
			if (auto *udialog = dynamic_cast<UpdatingDialog *>(dialog.get()))
				udialog->updateData();
		travelTab->update();
		inventoryTab->update();
		regionTab->update();
		conversionTab->update();
		marketTab->update();
	}

	void App::hackWindow() {
		if (mainWindow->get_width() == 0 && mainWindow->get_height() == 0) {
			delay(sigc::mem_fun(*this, &App::hackWindow));
		} else {
			Display *display = gdk_x11_display_get_xdisplay(gdk_display_get_default());
			Window window = gdk_x11_surface_get_xid(mainWindow->get_surface()->gobj());
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
	}
}
