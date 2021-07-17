#pragma once

#include <map>
#include <memory>
#include <set>
#include <stddef.h>
#include <string>
#include <unordered_set>
#include <utility>

#include "Area.h"
#include "Direction.h"
#include "Resource.h"
#include "platform.h"

class HousingArea;
class Game;

/** Regions are individual locations within the broader game universe. It's yet to be determined whether they're
 *  islands, planets or something else. Regions are divided into Areas. A Region can have only one Area of a given type.
 */
class Region {
	public:
		static constexpr const char *INVALID_CHARS = ":;?";
		struct Position {
			long x = 0, y = 0;
			Position(): Position(0, 0) {}
			Position(long x_, long y_): x(x_), y(y_) {}
			Position(const std::pair<long, long> &pair): Position(pair.first, pair.second) {}
			constexpr operator std::pair<long, long>() const { return {x, y}; }
			operator std::string() const { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }
			Position operator+(const Position &other) const { return {x + other.x, y + other.y}; }
			constexpr bool operator==(const Position &other) const { return x == other.x && y == other.y; }
			constexpr std::strong_ordering operator<=>(const Position &other) const {
				return std::pair<long, long>(*this) <=> std::pair<long, long>(other);
			}
		};

		Game *game;
		std::string name;
		Position position;
		size_t size = 0;
		/** Don't directly insert Areas into this map; use operator+= instead. */
		std::map<std::string, std::shared_ptr<Area>> areas;
		size_t money = 0;
		/** To get the buy price for a good, take the sell price and divide it by 1 plus the region's greed value. */
		double greed = 0.5;

		Region(Game &, const std::string &name_, const Position &, size_t size_);

		void tick(double delta);

		Resource::Map allResources() const;
		Resource::Map allOwnedResources() const;
		Resource::Map allNonOwnedResources() const;
		void subtractResourceFromNonOwned(const std::string &, double);
		size_t totalPopulation() const;
		bool occupied() const;
		std::shared_ptr<HousingArea> getHousing();
		std::shared_ptr<Area> getArea(Area::Type);
		bool hasNeighbor() const;
		bool hasNeighbor(Direction) const;
		Region * getNeighbor(Direction) const;
		std::unordered_set<Direction> validDirections() const;
		void erase(Area &);

		bool updateName(Area &, const std::string &);

		Region & setSize(size_t);
		Region & setMoney(size_t);
		Region & setPosition(const std::pair<s64, s64> &);
		Region & setName(const std::string &);

		Region & operator+=(std::shared_ptr<Area>);

		std::string toString() const;
		static std::unique_ptr<Region> fromString(Game &, const std::string &);
		static std::unique_ptr<Region> generate(Game &, const Position &, size_t);
		static std::unique_ptr<Region> generate(Game &, const Position &);
};
