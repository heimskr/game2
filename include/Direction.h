#pragma once

#include <utility>
#include "platform.h"

namespace Game2 {
	enum class Direction: int {North = 0, East, South, West};
	const char * toString(Direction);
}
