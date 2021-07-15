#include "area/Empty.h"
#include "Region.h"
#include "Game.h"

EmptyArea::EmptyArea(Region *region, size_t size): Area(region, size) {}

std::string EmptyArea::description() const {
	return "An empty area.";
}

void EmptyArea::tick(double delta) {}
