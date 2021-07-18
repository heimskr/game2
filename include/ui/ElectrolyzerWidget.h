#pragma once

#include "ui/ProcessorWidget.h"

namespace Game2 {
	class ElectrolyzerWidget: public ProcessorWidget {
		public:
			using ProcessorWidget::ProcessorWidget;

		protected:
			void addExtraButtons() override;
	};
}
