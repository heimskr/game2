#include "Game.h"
#include "Resource.h"
#include "processor/Refinery.h"


namespace Game2 {
	std::array<RefineryMode, 2> refineryModes = {RefineryMode::Clarify, RefineryMode::Polymerize};

	Refinery & Refinery::setMode(RefineryMode mode_) {
		mode = mode_;
		return *this;
	}

	Processor * Refinery::absorb(const nlohmann::json &json) {
		mode = json.at("mode");
		return this;
	}

	nlohmann::json Refinery::toJSON() const {
		auto out = Processor::toJSON();
		out["mode"] = mode;
		return out;
	}

	double Refinery::tick(double delta) {
		double out = 0.;

		const auto &recipes = game->recipes.refinery;

		for (const RefineryRecipe &recipe: recipes) {
			if (recipe.mode != mode)
				continue;
			bool enough = true;
			for (const auto &[name, amount]: recipe.inputs) {
				if (input.count(name) == 0 || ltna(input.at(name) * delta, amount * delta)) {
					enough = false;
					break;
				}
			}
			if (!enough)
				continue;
			for (const auto &[name, amount]: recipe.inputs)
				input[name] -= amount * delta;
			for (const auto &[name, amount]: recipe.outputs)
				output[name] += amount * delta;
		}

		shrink(input);
		moveOutput();
		return out;
	}

	const char * stringify(RefineryMode mode) {
		switch (mode) {
			case RefineryMode::Clarify:    return "Clarify";
			case RefineryMode::Polymerize: return "Polymerize";
			default: return "???";
		}
	}
}
