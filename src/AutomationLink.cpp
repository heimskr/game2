#include "AutomationLink.h"
#include "Game.h"
#include "Processor.h"
#include "Util.h"

namespace Game2 {
	AutomationLink::AutomationLink(Game &game_, std::shared_ptr<Processor> producer_, std::shared_ptr<Processor> consumer_,
	const std::string &resource_name, double weight_):
		game(&game_), producer(producer_), consumer(consumer_), resourceName(resource_name), weight(weight_) {}

	AutomationLink::AutomationLink(Game &game_, const nlohmann::json &json): game(&game_) {
		producer = game->processorsByID.at(json.at("producer"));
		consumer = game->processorsByID.at(json.at("consumer"));
		resourceName = json.at("resource");
		weight = json.at("weight");
	}

	std::string AutomationLink::toString() const {
		return producer->id + ":" + consumer->id + ":" + resourceName + ":" + std::to_string(weight);
	}

	void AutomationLink::tick() {
		if (producer->output.count(resourceName) == 0)
			return;
		consumer->input[resourceName] += producer->output.at(resourceName);
		producer->output.erase(resourceName);
	}

	void AutomationLink::setup() {
		producer->frozen.insert(resourceName);
	}

	void AutomationLink::cleanup() {
		producer->frozen.erase(resourceName);
	}

	void to_json(nlohmann::json &json, const AutomationLink &link) {
		json = {
			{"producer", link.producer->id},
			{"consumer", link.consumer->id},
			{"resource", link.resourceName},
			{"weight", link.weight},
		};
	}
}
