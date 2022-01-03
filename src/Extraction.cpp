#include "Area.h"
#include "Extraction.h"
#include "Game.h"
#include "Region.h"
#include "Util.h"

namespace Game2 {
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

	}
}
