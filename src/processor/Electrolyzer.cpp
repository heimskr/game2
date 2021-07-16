#include <list>

#include "Game.h"
#include "Resource.h"
#include "processor/Electrolyzer.h"

double Electrolyzer::tick(double delta) {
	double out = 0.;

	const auto &recipes = game->recipes.electrolyzer;

	for (auto &[name, amount]: input)
		if (recipes.count(name) != 0) {
			const std::vector<MultiOutput> &outputs = recipes.at(name).outputs;
			// TODO: possibly allow electrolyzer recipes to consume more than 1 of the resource per second
			const double to_convert = std::min(amount, delta);
			amount -= to_convert;
			for (const MultiOutput &result: outputs) {
				output[result.output] += to_convert * result.amount;
				out += to_convert * result.amount;
			}
		}

	shrink(input);
	moveOutput();
	return out;
}
