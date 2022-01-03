#include "Area.h"
#include "Extraction.h"
#include "Game.h"
#include "Region.h"
#include "Util.h"

namespace Game2 {
	Extraction::Extraction(const Game &game, const nlohmann::json &json): startAmount(json.at("startAmount")) {
		const Region *region = nullptr;
		const std::string &region_name = json.at("region");
		const std::string &area_name = json.at("area");
		for (const auto &pair: game.regions)
			if (pair.second->name == region_name)
				region = pair.second.get();
		if (!region)
			throw std::runtime_error("Couldn't find Region for extraction");
		if (region->areas.count(area_name) == 0)
			throw std::runtime_error("Couldn't find Area for extraction");
		area = const_cast<Area *>(region->areas.at(area_name).get());
		resourceName = json.at("resource");
		amount = json.at("amount");
		rate = json.at("rate");
		minimum = json.at("minimum");
	}

	void to_json(nlohmann::json &json, const Extraction &extraction) {
		json = {
			{"region", extraction.area->parent->name},
			{"area", extraction.area->name},
			{"resource", extraction.resourceName},
			{"amount", extraction.amount},
			{"rate", extraction.rate},
			{"startAmount", extraction.startAmount},
			{"minimum", extraction.minimum},
		};
	}
}
