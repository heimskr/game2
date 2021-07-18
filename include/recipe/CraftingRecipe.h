#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace Game2 {
	struct CraftingRecipe {
		const std::map<std::string, double> inputs;
		const std::string output;
		const double amount;

		CraftingRecipe(const std::map<std::string, double> &inputs_, const char *output_, double amount_);
		CraftingRecipe(std::map<std::string, double> &&inputs_, const char *output_, double amount_);
	};
}
