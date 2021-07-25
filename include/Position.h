#pragma once

#include <cstdint>
#include <string>
#include <utility>

#include "Direction.h"

namespace Game2 {
	struct Position {
		int64_t x = 0, y = 0;
		Position(): Position(0, 0) {}
		Position(int64_t x_, int64_t y_): x(x_), y(y_) {}
		Position(const std::pair<int64_t, int64_t> &pair): Position(pair.first, pair.second) {}
		Position(Direction);
		constexpr operator std::pair<int64_t, int64_t>() const { return {x, y}; }
		operator std::string() const { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }
		Position operator+(const Position &other) const { return {x + other.x, y + other.y}; }
		constexpr bool operator==(const Position &other) const { return x == other.x && y == other.y; }
		constexpr bool operator<(const Position &other) const {
			return std::pair<int64_t, int64_t>(*this) < std::pair<int64_t, int64_t>(other);
		}
	};
}
