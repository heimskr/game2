#pragma once

#include "ui/ProcessorWidget.h"

namespace Game2 {
	class RocketFurnaceWidget: public ProcessorWidget {
		public:
			using ProcessorWidget::ProcessorWidget;

			virtual void update() override;

		protected:
			void addExtraButtons() override;

		private:
			Gtk::Button fillButton;
			Gtk::Label fuelLabel;
	};
}
