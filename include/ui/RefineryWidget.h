#pragma once

#include "ui/ProcessorWidget.h"

namespace Game2 {
	class RefineryWidget: public ProcessorWidget {
		public:
			using ProcessorWidget::ProcessorWidget;

			void update() override;

		protected:
			void addExtraButtons() override;

		private:
			Gtk::Button modeButton;
			Gtk::Label modeLabel {"", Gtk::Align::START};
	};
}
