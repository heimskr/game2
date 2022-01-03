#include "Processor.h"
#include "Resource.h"
#include "Util.h"
#include "processor/Processors.h"
#include "Game.h"

namespace Game2 {
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

	nlohmann::json Processor::toJSON() const {
		return {
			{"type", int(getType())},
			{"input", input},
			{"output", output},
			{"autoExtract", autoExtract},
			{"name", name},
			{"id", id},
		};
	}

	std::shared_ptr<Processor> Processor::fromJSON(Game &game, const nlohmann::json &json) {
		Processor *out = nullptr;
		const Type type = json.at("type");

		switch (type) {
			case Type::Furnace:       out = (new       Furnace(game))->absorb(json); break;
			case Type::Centrifuge:    out = (new    Centrifuge(game))->absorb(json); break;
			case Type::Fermenter:     out = (new     Fermenter(game))->absorb(json); break;
			case Type::Crusher:       out = (new       Crusher(game))->absorb(json); break;
			case Type::Refinery:      out = (new      Refinery(game))->absorb(json); break;
			case Type::RocketFurnace: out = (new RocketFurnace(game))->absorb(json); break;
			case Type::Electrolyzer:  out = (new  Electrolyzer(game))->absorb(json); break;
			default: throw std::invalid_argument("Invalid Processor type: " + std::to_string(int(type)));
		}

		out->setInput(json.at("input")).setOutput(json.at("output")).setAutoExtract(json.at("autoExtract"));
		out->setName(json.at("name")).setID(json.at("id"));
		return std::shared_ptr<Processor>(out);
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

	void to_json(nlohmann::json &json, const Processor &processor) {
		json = processor.toJSON();
	}

	void to_json(nlohmann::json &json, const std::shared_ptr<Processor> &processor) {
		to_json(json, *processor);
	}
}
