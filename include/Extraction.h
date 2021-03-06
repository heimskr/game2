#pragma once

#include <string>

#include "json.hpp"

namespace Game2 {
	class Area;
	class Game;

	struct Extraction {
		Area *area = nullptr;
		std::string resourceName;
		const double startAmount;
		double amount;
		double rate;
		/** Extraction won't cause the amount of the resource present to be less than this number. */
		double minimum = 0.;

		Extraction(Area *area_, const std::string &resource_name, double start_amount, double amount_, double rate_,
		           double minimum_):
			area(area_), resourceName(resource_name), startAmount(start_amount), amount(amount_), rate(rate_),
			minimum(minimum_) {}

		Extraction(const Game &, const nlohmann::json &);
	};

	void to_json(nlohmann::json &, const Extraction &);
}
