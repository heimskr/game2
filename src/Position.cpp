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

	void to_json(nlohmann::json &json, const Position &position) {
		json = {position.x, position.y};
	}

	void from_json(const nlohmann::json &json, Position &position) {
		position.x = json.at(0);
		position.y = json.at(1);
	}
}
