#include "area/Forest.h"
#include "Region.h"
#include "Game.h"

namespace Game2 {
	ForestArea::ForestArea(Region *region, size_t size): Area(region, size) {
		resources.emplace("Wood", static_cast<double>(size));
	}

	std::string ForestArea::description() const {
		return "A quiet wooded area.";
	}

	void ForestArea::tick(double delta) {
		double &wood = resources["Wood"];
		if ((wood += wood * .02 * delta) > static_cast<double>(size))
			wood = static_cast<double>(size);
	}
}
