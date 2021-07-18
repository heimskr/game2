#include "recipe/RefineryRecipe.h"

namespace Game2 {
	RefineryRecipe::RefineryRecipe(RefineryMode mode_, const Group &inputs_, const Group &outputs_):
		mode(mode_), inputs(inputs_), outputs(outputs_) {}

	RefineryRecipe::RefineryRecipe(RefineryMode mode_, Group &&inputs_, Group &&outputs_):
		mode(mode_), inputs(std::move(inputs_)), outputs(std::move(outputs_)) {}
}
