#pragma once

#include "ui/ProcessorWidget.h"

namespace Game2 {
	class FurnaceWidget: public ProcessorWidget {
		public:
			using ProcessorWidget::ProcessorWidget;

		protected:
			void addExtraButtons() override;

		private:
			Gtk::Button fillButton;
	};
}
