#pragma once

#include "ui/ProcessorWidget.h"

class RefineryWidget: public ProcessorWidget {
	public:
		using ProcessorWidget::ProcessorWidget;

	protected:
		void addExtraButtons() override;

	private:
		Gtk::Button modeButton;
};
