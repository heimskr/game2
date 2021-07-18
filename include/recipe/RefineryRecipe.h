#pragma once

#include <string>
#include <utility>
#include <vector>

#include "recipe/RandomRecipe.h"
#include "defs/RefineryMode.h"

namespace Game2 {
	struct RefineryRecipe {
		using Group = std::vector<std::pair<std::string, double>>;
		const RefineryMode mode;
		const Group inputs;
		const Group outputs;

		RefineryRecipe(RefineryMode, const Group &inputs_, const Group &outputs_);
		RefineryRecipe(RefineryMode, Group &&inputs_, Group &&outputs_);
	};
}
