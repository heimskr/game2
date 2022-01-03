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
#include "json.hpp"
#include "Position.h"
#include "Resource.h"

namespace Game2 {
	class HousingArea;
	class Game;

	/** Regions are individual locations within the broader game universe. It's yet to be determined whether they're
	 *  islands, planets or something else. Regions are divided into Areas. A Region can have only one Area of a given
	 *  type. */
	class Region {
		public:
			static constexpr const char *INVALID_CHARS = ":;?";

			Game *game;
			std::string name;
			Position position;
			size_t size = 0;
			/** Don't directly insert Areas into this map; use operator+= instead. */
			std::map<std::string, std::shared_ptr<Area>> areas;
			size_t money = 0;
			/** To get the buy price for a good, take the sell price and divide it by 1 plus the region's greed
			 *  value. */
			double greed = 0.5;

			Region(Game &, const std::string &name_, const Position &, size_t size_);
			Region(Game &, const nlohmann::json &);

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
			Region & setPosition(const Position &);
			Region & setName(const std::string &);

			Region & operator+=(std::shared_ptr<Area>);

			static std::unique_ptr<Region> generate(Game &, const Position &, size_t);
			static std::unique_ptr<Region> generate(Game &, const Position &);
	};

	void to_json(nlohmann::json &, const Region &);
}
