#include <stdexcept>

#include "Position.h"

namespace Game2 {
	Position::Position(Direction direction) {
		switch (direction) {
			case Direction::North: x =  0; y = -1; break;
			case Direction::East:  x =  1; y =  0; break;
			case Direction::South: x =  0; y =  1; break;
			case Direction::West:  x = -1; y =  0; break;
			default: throw std::invalid_argument("Invalid Direction");
		}
	}
}
