#include "App.h"
#include "UI.h"
#include "ui/TravelTab.h"

TravelTab::TravelTab(App &app_): app(app_) {
	grid.set_row_homogeneous(true);
	grid.set_column_homogeneous(true);
	grid.set_row_spacing(5);
	grid.set_column_spacing(5);
	setMargins(grid, 5);
	for (int row = 0; row < ROWS; ++row)
		for (int column = 0; column < COLUMNS; ++column)
			grid.attach(buttons[row * COLUMNS + column], column, row);
}
