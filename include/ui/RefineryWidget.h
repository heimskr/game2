#pragma once

#include "ui/ProcessorWidget.h"

namespace Game2 {
	class RefineryWidget: public ProcessorWidget {
		public:
			using ProcessorWidget::ProcessorWidget;

		protected:
			void addExtraButtons() override;

		private:
			Gtk::Button modeButton;
	};
}
