#include "Game.h"
#include "processor/Refinery.h"
#include "tab/ConversionTab.h"
#include "ui/AppWindow.h"
#include "ui/RefineryModesDialog.h"
#include "ui/RefineryWidget.h"

namespace Game2 {
	void RefineryWidget::update() {
		modeLabel.set_text("(" + std::string(stringify(static_cast<Refinery &>(processor).mode)) + ")");
	}

	void RefineryWidget::addExtraButtons() {
		modeButton.insert_before(topBox, nameLabel);
		modeButton.set_icon_name("preferences-system-symbolic");
		modeButton.set_tooltip_text("Set refinery mode");
		modeButton.signal_clicked().connect([this] {
			auto *dialog = new RefineryModesDialog("Refinery Mode", appWindow);
			appWindow.dialog.reset(dialog);
			dialog->signal_submit().connect([this](RefineryMode mode) {
				static_cast<Refinery &>(processor).setMode(mode);
				appWindow.conversionTab->update();
			});
			appWindow.dialog->show();
		});
		topBox.append(modeLabel);
		update();
	}
}
