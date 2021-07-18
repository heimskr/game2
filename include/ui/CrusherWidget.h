#pragma once

#include "ui/ProcessorWidget.h"

class CrusherWidget: public ProcessorWidget {
	public:
		using ProcessorWidget::ProcessorWidget;

	protected:
		void addExtraButtons() override;

	private:
		Gtk::Button fillButton;
};
