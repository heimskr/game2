#include <list>

#include "Game.h"
#include "Resource.h"
#include "processor/Centrifuge.h"

namespace Game2 {
	double Centrifuge::tick(double delta) {
		double out = 0.;

		const auto &recipes = game->recipes.centrifuge;

		for (auto &[rname, amount]: input)
			if (recipes.count(rname) != 0) {
				const std::pair<std::string, double> choice = recipes.at(rname).choose();
				// TODO: possibly allow centrifuge recipes to consume more than 1 of the resource per second
				const double to_convert = std::min(amount, delta);
				amount -= to_convert;
				if (!choice.first.empty()) {
					output[choice.first] += to_convert * choice.second;
					out += to_convert;
				}
			}

		shrink(input);
		moveOutput();
		return out;
	}
}
