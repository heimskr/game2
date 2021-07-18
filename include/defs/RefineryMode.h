#pragma once

#include <array>

namespace Game2 {
	enum class RefineryMode {Clarify, Polymerize};
	extern std::array<RefineryMode, 2> refineryModes;
	const char * stringify(RefineryMode);
}
