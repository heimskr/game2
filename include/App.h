#pragma once

#include "Toolkit.h"
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_set>

#include "Game.h"
#include "ui/AppWindow.h"

namespace Game2 {
	class App: public Gtk::Application {
		public:
			Glib::RefPtr<Gtk::Builder> builder;

			static Glib::RefPtr<App> create();

			void on_startup() override;
			void on_activate() override;

			AppWindow * create_window();

		protected:
			App();

		private:
			void on_hide_window(Gtk::Window *window);
	};

	extern Glib::RefPtr<App> global_app;
}
