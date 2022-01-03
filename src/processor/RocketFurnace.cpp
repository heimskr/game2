#include <list>

#include "Game.h"
#include "Resource.h"
#include "processor/RocketFurnace.h"

namespace Game2 {
	RocketFurnace & RocketFurnace::setHydrogen(double hydrogen_) {
		hydrogen = hydrogen_;
		return *this;
	}

	RocketFurnace & RocketFurnace::setOxygen(double oxygen_) {
		oxygen = oxygen_;
		return *this;
	}

	Processor * RocketFurnace::absorb(const nlohmann::json &json) {
		hydrogen = json.at("hydrogen");
		oxygen = json.at("oxygen");
		return this;
	}

	nlohmann::json RocketFurnace::toJSON() const {
		auto out = Processor::toJSON();
		out["hydrogen"] = hydrogen;
		out["oxygen"] = oxygen;
		return out;
	}

	double RocketFurnace::tick(double delta) {
		if (input.count("Hydrogen") != 0) {
			double &amount = input.at("Hydrogen");
			const double to_remove = std::min(delta * 20., amount);
			amount -= to_remove;
			hydrogen += to_remove;
		}

		if (input.count("Oxygen") != 0) {
			double &amount = input.at("Oxygen");
			const double to_remove = std::min(delta * 10., amount);
			amount -= to_remove;
			oxygen += to_remove;
		}

		double out = 0.;

		if (hydrogen < Resource::MIN_AMOUNT / 2.) {
			hydrogen = 0.;
		} else if (oxygen < Resource::MIN_AMOUNT) {
			oxygen = 0.;
		} else {
			for (auto &[name, amount]: input) {
				const Resource &resource = game->resources.at(name);
				double to_convert = 0;
				if (resource.conversions.count(Type::RocketFurnace) != 0) {
					const auto &conversion = resource.conversions.at(Type::RocketFurnace);
					to_convert = std::min(hydrogen / 2., std::min(oxygen, std::min(amount,
						conversion.rate * 10. * delta)));
					output[conversion.outName] += to_convert * conversion.amount;
				} else if (resource.conversions.count(Type::Furnace) != 0) {
					const auto &conversion = resource.conversions.at(Type::Furnace);
					to_convert = std::min(hydrogen / 2., std::min(oxygen, std::min(amount,
						conversion.rate * 10. * delta)));
					output[conversion.outName] += to_convert * conversion.amount;
				} else {
					continue;
				}
				out += to_convert;
				hydrogen -= to_convert * 2.;
				oxygen -= to_convert;
				amount -= to_convert;
				if (hydrogen < Resource::MIN_AMOUNT * 2. || oxygen < Resource::MIN_AMOUNT)
					break;
			}
			moveOutput();
		}

		shrink(input);
		return out;
	}
}
