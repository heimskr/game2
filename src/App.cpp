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

	AppWindow * App::create_window() {
		AppWindow *window = AppWindow::create();
		add_window(*window);
		window->tickThread = std::thread([window] {
			while (window->alive) {
				{
					auto lock = window->lockGame();
					if (window->game) {
						window->game->tick(UPDATE_PERIOD / 1000.);
						window->updateDispatcher.emit();
					}
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_PERIOD));
			}
		});
		window->signal_hide().connect(sigc::bind(sigc::mem_fun(*this, &App::on_hide_window), window));
		return window;
	}

	void App::on_startup() {
		Gtk::Application::on_startup();
	}

	void App::on_activate() {
		try {
			auto window = create_window();
			window->signal_show().connect([this, window] {
				window->delay(sigc::mem_fun(*window, &AppWindow::hackWindow));
			});

			window->present();
		} catch (const Glib::Error &err) {
			std::cerr << "App::on_activate(): " << err.what() << std::endl;
		} catch (const std::exception &err) {
			std::cerr << "App::on_activate(): " << err.what() << std::endl;
		}
	}

	void App::on_hide_window(Gtk::Window *window) {
		delete window;
	}
}
