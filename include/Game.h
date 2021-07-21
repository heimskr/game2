#pragma once

#include <list>
#include <map>
#include <memory>

#include "AutomationLink.h"
#include "Extraction.h"
#include "RecipeManager.h"
#include "Region.h"
#include "Resource.h"
#include "Util.h"

namespace Game2 {
	class App;
	class Processor;

	class Game {
		public:
			App &app;
			bool ready = false;
			std::map<std::string, Resource> resources;
			std::map<Processor::Type, Resource::Map> processorCosts;
			RecipeManager recipes = {*this};

			std::map<std::string, double> inventory, craftingInventory;
			std::map<Position, std::shared_ptr<Region>> regions;
			std::list<Extraction> extractions;
			std::list<std::shared_ptr<Processor>> processors;
			std::unordered_map<std::string, std::shared_ptr<Processor>> processorsByID;
			std::list<AutomationLink> automationLinks;
			Position position;
			size_t money = 0;
			bool cheatsEnabled = false;

			Game() = delete;
			Game(App &);

			void add(const Resource &);
			void addResources();
			void add(Processor::Type, const Resource::Map &);
			void addProcessorCosts();
			void addAll();
			std::string randomResource(const Resource::Type &) const;

			bool updatePosition(Region &, const Position &);
			Position suggestPosition(bool is_free, s64 x = 0, s64 y = 0);
			bool updateName(Region &, const std::string &);

			Region & currentRegion();
			std::shared_ptr<Region> currentRegionPointer();
			bool erase(Region &);
			void eraseExtractions(const Area &);
			void setMoney(size_t);
			void addToInventory(const std::string &resource_name, double amount);

			void tick(double delta);

			static std::shared_ptr<Game> loadDefaults(App &);

			void extract(Area &, const std::string &name, double amount, double minimum);
			void extract(Area &, const std::string &name, double amount, double minimum, double rate);

			std::string toString() const;
			static std::shared_ptr<Game> fromString(App &, const std::string &);
			static std::shared_ptr<Game> load(App &);
			void save();

			Game & operator+=(std::unique_ptr<Region> &&);
			Extraction * getExtraction(const Area &, const std::string &);
			const Extraction * getExtraction(const Area &, const std::string &) const;
	};
}
