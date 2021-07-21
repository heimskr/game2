#pragma once

#include <string>
#include <utility>

#include "Direction.h"

namespace Game2 {
	struct Position {
		long x = 0, y = 0;
		Position(): Position(0, 0) {}
		Position(long x_, long y_): x(x_), y(y_) {}
		Position(const std::pair<long, long> &pair): Position(pair.first, pair.second) {}
		Position(Direction);
		constexpr operator std::pair<long, long>() const { return {x, y}; }
		operator std::string() const { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }
		Position operator+(const Position &other) const { return {x + other.x, y + other.y}; }
		constexpr bool operator==(const Position &other) const { return x == other.x && y == other.y; }
		constexpr std::strong_ordering operator<=>(const Position &other) const {
			return std::pair<long, long>(*this) <=> std::pair<long, long>(other);
		}
	};
}
