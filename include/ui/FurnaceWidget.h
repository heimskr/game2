#pragma once

#include "ui/ProcessorWidget.h"

namespace Game2 {
	class FurnaceWidget: public ProcessorWidget {
		public:
			using ProcessorWidget::ProcessorWidget;

			void update() override;

		protected:
			void addExtraButtons() override;

		private:
			double lastFuel = -1;
			Gtk::Button fillButton;
			Gtk::Label fuelLabel;
	};
}
