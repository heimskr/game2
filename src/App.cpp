#include <filesystem>
#include <iostream>

#include "App.h"
#include "UI.h"

namespace Game2 {
	Glib::RefPtr<App> global_app;

	App::App(): Gtk::Application("com.heimskr.game2", Gio::Application::Flags::HANDLES_OPEN) {}

	Glib::RefPtr<App> App::create() {
		return Glib::make_refptr_for_instance<App>(new App());
	}

	void App::on_startup() {
		Gtk::Application::on_startup();
		set_accel_for_action("win.new", "<Ctrl>n");
		set_accel_for_action("win.open", "<Ctrl>o");
		set_accel_for_action("win.save", "<Ctrl>s");
		set_accel_for_action("win.save_as", "<Ctrl><Shift>s");
	}

	void App::on_activate() {
		try {
			auto window = create_window();
			window->signal_show().connect([window] {
				window->delay(sigc::mem_fun(*window, &AppWindow::hackWindow));
			});
			window->present();
		} catch (const Glib::Error &err) {
			std::cerr << "App::on_activate(): " << err.what() << std::endl;
		} catch (const std::exception &err) {
			std::cerr << "App::on_activate(): " << err.what() << std::endl;
		}
	}

	AppWindow * App::create_window() {
		AppWindow *window = AppWindow::create();
		add_window(*window);
		window->tickThread = std::thread([window] {
			while (window->alive) {
				{
					auto lock = window->lockGame();
					if (window->game) {
						window->game->tick(window->updatePeriod / 1000.);
						window->updateDispatcher.emit();
					}
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(window->updatePeriod));
			}
		});
		window->signal_hide().connect(sigc::bind(sigc::mem_fun(*this, &App::on_hide_window), window));
		return window;
	}

	const char * App::get_text(const std::string &path, gsize &size) {
		return reinterpret_cast<const char *>(Gio::Resource::lookup_data_global(path)->get_data(size));
	}

	const char * App::get_text(const std::string &path) {
		gsize size;
		return get_text(path, size);
	}

	void App::on_hide_window(Gtk::Window *window) {
		delete window;
	}
}
