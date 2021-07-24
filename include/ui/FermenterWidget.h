#pragma once

#include "ui/ProcessorWidget.h"

namespace Game2 {
	class FermenterWidget: public ProcessorWidget {
		public:
			using ProcessorWidget::ProcessorWidget;

			void update() override;

		protected:
			void addExtraButtons() override;

		private:
			double lastYeast = -1.;
			Gtk::Button fillButton;
			Gtk::Label yeastLabel;
	};
}
