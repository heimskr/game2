#include "Processor.h"
#include "Resource.h"
#include "Util.h"
#include "processor/Processors.h"
#include "Game.h"
#include "main.h"

Processor::Processor(Game &game_): game(&game_) {}

Processor & Processor::setInput(const std::map<std::string, double> &input_) {
	input = input_;
	return *this;
}

Processor & Processor::setOutput(const std::map<std::string, double> &output_) {
	output = output_;
	return *this;
}

Processor & Processor::setInput(std::map<std::string, double> &&input_) {
	input = std::move(input_);
	return *this;
}

Processor & Processor::setOutput(std::map<std::string, double> &&output_) {
	output = std::move(output_);
	return *this;
}

Processor & Processor::setAutoExtract(bool auto_extract) {
	autoExtract = auto_extract;
	return *this;
}

Processor & Processor::setName(const std::string &name_) {
	name = name_;
	return *this;
}

Processor & Processor::setName(std::string &&name_) {
	name = std::move(name_);
	return *this;
}

Processor & Processor::setID(const std::string &id_) {
	id = id_;
	return *this;
}

Processor & Processor::setID(std::string &&id_) {
	id = std::move(id_);
	return *this;
}

double Processor::tick(double delta) {
	double out = 0.;

	for (auto &[name, amount]: input) {
		const Resource &resource = game->resources.at(name);
		if (resource.conversions.count(getType()) != 0) {
			const auto &conversion = resource.conversions.at(getType());
			const double to_convert = std::min(amount, conversion.rate * delta);
			out += to_convert;
			amount -= to_convert;
			output[conversion.outName] += to_convert * conversion.amount;
		}
	}

	shrink(input);
	moveOutput();
	return out;
}

void Processor::moveOutput() {
	if (autoExtract) {
		std::list<const std::string *> to_remove;
		for (auto &[name, amount]: output)
			if (frozen.count(name) == 0) {
				game->inventory[name] += amount;
				to_remove.push_back(&name);
			}
		for (const std::string *name: to_remove)
			output.erase(*name);
	}
}

std::string Processor::toString() const {
	return std::to_string(static_cast<int>(getType())) + ":" + stringify(input) + ":" + stringify(output) + ":"
		+ (autoExtract? "1" : "0") + ":" + name + ":" + id;
}

Processor * Processor::fromString(Game &game, const std::string &str) {
	std::vector<std::string> pieces = split(str, ":", false);
	Resource::Map input(parseMap(pieces[1])), output(parseMap(pieces[2]));
	const bool auto_extract = parseLong(pieces[3]) != 0;
	std::string &name = pieces[4];
	std::string &id = pieces[5];
	const std::string &extra = pieces[6];
	const std::string &extra2 = pieces[7];
	const Type type = static_cast<Type>(parseLong(pieces[0]));
	Processor *out = nullptr;
	switch (type) {
		case Type::Furnace:       out = &((new Furnace(game))->setFuel(parseDouble(extra))); break;
		case Type::Centrifuge:    out = new Centrifuge(game); break;
		case Type::Fermenter:     out = &((new Fermenter(game))->setYeast(parseDouble(extra))); break;
		case Type::Crusher:       out = new Crusher(game); break;
		case Type::Refinery:      out = &((new Refinery(game))->setMode(static_cast<RefineryMode>(parseLong(extra)))); break;
		case Type::RocketFurnace: out = &((new RocketFurnace(game))->setHydrogen(parseDouble(extra)).setOxygen(parseDouble(extra2))); break;
		case Type::Electrolyzer:  out = new Electrolyzer(game); break;
		default: throw std::invalid_argument("Invalid Processor type: " + std::to_string(static_cast<int>(type)));
	}

	out->setInput(std::move(input)).setOutput(std::move(output)).setAutoExtract(auto_extract).setName(std::move(name));
	out->setID(std::move(id));
	return out;
}

Processor * Processor::ofType(Game &game, Type type) {
	Processor *out;
	switch (type) {
		case Type::Furnace:       out = new Furnace(game);       break;
		case Type::Centrifuge:    out = new Centrifuge(game);    break;
		case Type::Fermenter:     out = new Fermenter(game);     break;
		case Type::Crusher:       out = new Crusher(game);       break;
		case Type::Refinery:      out = new Refinery(game);      break;
		case Type::RocketFurnace: out = new RocketFurnace(game); break;
		case Type::Electrolyzer:  out = new Electrolyzer(game);  break;
		default: throw std::invalid_argument("Invalid Processor type: " + std::to_string(static_cast<int>(type)));
	}
	out->setID(makeUUID());
	return out;
}

const char * Processor::typeName(Type type) {
	switch (type) {
		case Type::Furnace:       return "Furnace";
		case Type::Centrifuge:    return "Centrifuge";
		case Type::Fermenter:     return "Fermenter";
		case Type::Crusher:       return "Crusher";
		case Type::Refinery:      return "Refinery";
		case Type::RocketFurnace: return "Rocket Furnace";
		case Type::Electrolyzer:  return "Electrolyzer";
		default: return "?";
	}
}
