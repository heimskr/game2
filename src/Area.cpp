#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Area.h"
#include "Region.h"
#include "area/Areas.h"
#include "Util.h"

namespace Game2 {
	Area::Area(Region *parent_, size_t size_): parent(parent_), size(size_) {}

	Area & Area::setResources(const Resource::Map &resources_) {
		resources = resources_;
		return *this;
	}

	Area & Area::setResources(Resource::Map &&resources_) {
		resources = std::move(resources_);
		return *this;
	}

	Area & Area::setSize(size_t size_) {
		size = size_;
		return *this;
	}

	Area & Area::setPlayerOwned(bool player_owned) {
		playerOwned = player_owned;
		return *this;
	}

	Area & Area::setName(const std::string &name_) {
		if (name_.find_first_of(INVALID_CHARS) != std::string::npos)
			throw std::invalid_argument("Invalid area name");
		name = name_;
		return *this;
	}

	Area & Area::setName(std::string &&name_) {
		if (name_.find_first_of(INVALID_CHARS) != std::string::npos)
			throw std::invalid_argument("Invalid area name");
		name = std::move(name_);
		return *this;
	}

	bool Area::reduceSize(size_t new_size) {
		if (new_size == 0 || getType() == Type::Empty || size < new_size)
			return false;
		if (size == new_size)
			return true;

		std::shared_ptr<Area> empty = parent->getArea(Type::Empty);

		if (!empty) {
			empty = std::make_shared<EmptyArea>(parent, size - new_size);
			empty->setName("Empty");
			*parent += empty;
		} else
			empty->setSize(empty->size + size - new_size);

		size = new_size;
		return true;
	}

	size_t Area::totalPopulation() const {
		size_t out = 0;
		for (const auto &pair: resources)
			if (Resource::hasType(*parent->game, pair.first, "sapient"))
				out += static_cast<size_t>(pair.second);
		return out;
	}

	std::shared_ptr<Area> Area::fromJSON(Region &region, const nlohmann::json &json) {
		std::shared_ptr<Area> area;
		const Type type = Type(json.at("type").get<int>());
		switch (Type(type)) {
			case Type::Housing:  area = std::make_shared<HousingArea>(&region);  break;
			case Type::Forest:   area = std::make_shared<ForestArea>(&region);   break;
			case Type::Mountain: area = std::make_shared<MountainArea>(&region); break;
			case Type::Lake:     area = std::make_shared<LakeArea>(&region);     break;
			case Type::Empty:    area = std::make_shared<EmptyArea>(&region);    break;
			case Type::Farmland: area = std::make_shared<FarmlandArea>(&region, 0, json.at("resource")); break;
			case Type::OilField: area = std::make_shared<OilFieldArea>(&region); break;
			default: throw std::invalid_argument("Unknown Area type: " + std::to_string(int(type)));
		}

		area->setName(json.at("name")).setSize(json.at("size")).setPlayerOwned(json.at("playerOwned"));
		area->setResources(json.at("resources"));
		return area;
	}

	nlohmann::json Area::toJSON() const {
		return {
			{"name", name},
			{"size", size},
			{"playerOwned", playerOwned},
			{"type", int(getType())},
			{"resources", resources},
		};
	}

	void to_json(nlohmann::json &json, const Area &area) {
		json = area.toJSON();
	}

	void to_json(nlohmann::json &json, const std::shared_ptr<Area> &area) {
		to_json(json, *area);
	}
}