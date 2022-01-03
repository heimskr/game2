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
		resourceName = json.at("resourceName");
		amount = json.at("amount");
		rate = json.at("rate");
		minimum = json.at("minimum");
	}

	std::string Extraction::toString() const {
		return area->parent->name + ":" + area->name + ":" + resourceName + ":" + std::to_string(amount) + ":"
			+ std::to_string(rate) + ":" + std::to_string(startAmount) + ":" + std::to_string(minimum);
	}

	Extraction Extraction::fromString(const Game &game, const std::string &str) {
		const std::vector<std::string> pieces = split(str, ":", false);
		if (pieces.size() != 7)
			throw std::invalid_argument("Expected 7 pieces in Extraction::fromString");
		const Region *region = nullptr;
		for (const auto &pair: game.regions)
			if (pair.second->name == pieces[0])
				region = pair.second.get();
		if (!region)
			throw std::runtime_error("Couldn't find Region for extraction");
		if (region->areas.count(pieces[1]) == 0)
			throw std::runtime_error("Couldn't find Area for extraction");
		const Area *area = region->areas.at(pieces[1]).get();
		const double amount = parseDouble(pieces[3]);
		const double start_amount = parseDouble(pieces[5]);
		return {
			const_cast<Area *>(area), // Area
			pieces[2],                // Resource name
			start_amount,             // Start amount
			amount,                   // Amount
			parseDouble(pieces[4]),   // Rate
			parseDouble(pieces[6])    // Minimum
		};
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
