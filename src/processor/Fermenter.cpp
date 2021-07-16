#include <list>

#include "Game.h"
#include "Resource.h"
#include "processor/Fermenter.h"

Fermenter & Fermenter::setYeast(double yeast_) {
	yeast = yeast_;
	return *this;
}

std::string Fermenter::toString() const {
	return Processor::toString() + ":" + std::to_string(yeast);
}

double Fermenter::tick(double delta) {
	if (input.count("Yeast") != 0) {
		double &amount = input.at("Yeast");
		const double to_remove = std::min(delta, amount);
		amount -= to_remove;
		yeast += to_remove;
	}

	double out = 0.;

	if (yeast < Resource::MIN_AMOUNT) {
		yeast = 0.;
	} else {
		for (auto &[name, amount]: input) {
			const Resource &resource = game->resources.at(name);
			if (resource.conversions.count(getType()) == 0)
				continue;
			const auto &conversion = resource.conversions.at(getType());
			const double to_convert = std::min(yeast, std::min(amount, conversion.rate * delta));
			out += to_convert;
			yeast -= to_convert;
			amount -= to_convert;
			output[conversion.outName] += to_convert * conversion.amount;
			if (yeast < Resource::MIN_AMOUNT)
				break;
		}
		moveOutput();
	}

	shrink(input);
	return out;
}
