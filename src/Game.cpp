#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include "platform.h"

#include "FS.h"
#include "Game.h"
#include "NameGen.h"
#include "Util.h"
#include "Processor.h"
#include "area/Areas.h"
#include "processor/Processors.h"
#include "ui/AppWindow.h"

namespace Game2 {
	Game::Game(AppWindow &window_, const std::string &path_): window(window_), path(path_) {
		addAll();
	}

	void Game::add(const Resource &resource) {
		resources.emplace(resource.name, resource);
	}

	static constexpr double ELEMENT_BASE = 3.;

	long double operator""_e(long double price) {
		return price / 0.424 * ELEMENT_BASE;
		// 0.424: Price of iron (USD/kg).
		// I still need to decide whether units in this game represent mass or volume.
	}

	void Game::addResources() {
		add(Resource(this, "Apple").setBasePrice(0.5).addTypes("farmable", "food", "centrifugable").add(Processor::Type::Fermenter, {0.1, "Cider"}));
		add(Resource(this, "Beer").setBasePrice(20.).addTypes("drink", "alcohol", "liquid"));
		add(Resource(this, "Blood").setBasePrice(10.).addTypes("liquid", "electrolyzable"));
		add(Resource(this, "Cellulose").setBasePrice(1.0).addTypes("molecule", "chemical", "electrolyzable"));
		add(Resource(this, "Charcoal").setBasePrice(1.5).addTypes("fuel"));
		add(Resource(this, "Cider").setBasePrice(18.).addTypes("drink", "alcohol", "liquid"));
		add(Resource(this, "Coal").setBasePrice(2.).addTypes("fuel", "smeltable", "electrolyzable").setDefaultExtractionRate(0.5));
		add(Resource(this, "Crude Oil").setBasePrice(5.));
		add(Resource(this, "Fish").setBasePrice(5.).addTypes("living", "centrifugable"));
		add(Resource(this, "Glass").setBasePrice(1.));
		add(Resource(this, "Honey").setBasePrice(0.75).addTypes("farmable", "food").add(Processor::Type::Fermenter, {0.1, "Mead"}));
		add(Resource(this, "Hops").setBasePrice(0.4).addTypes("farmable").add(Processor::Type::Fermenter, {0.1, "Beer"}));
		add(Resource(this, "Human").setBasePrice(9999.).addTypes("alive", "sapient"));
		add(Resource(this, "Iron Ore").setBasePrice(2.).addTypes("ore", "iron", "smeltable", "rocket smeltable").add(Processor::Type::Furnace, {0.5, "Iron"}).setDefaultExtractionRate(0.5));
		add(Resource(this, "Lignin").setBasePrice(10.0).addTypes("molecule", "chemical", "electrolyzable"));
		add(Resource(this, "Malic Acid").setBasePrice(1.0).addTypes("molecule", "chemical", "electrolyzable"));
		add(Resource(this, "Mead").setBasePrice(25.).addTypes("drink", "alcohol", "liquid"));
		add(Resource(this, "Microchip").setBasePrice(300.).addTypes("rare spawnable"));
		add(Resource(this, "Plastic").setBasePrice(2.5));
		add(Resource(this, "Sand").setBasePrice(0.6).addTypes("smeltable", "rocket smeltable").add(Processor::Type::Furnace, {0.25, "Glass"}));
		add(Resource(this, "Stone").setBasePrice(0.1).setDefaultExtractionRate(0.5).addTypes("centrifugable"));
		add(Resource(this, "Water").setBasePrice(0.05).addTypes("liquid", "centrifugable", "electrolyzable"));
		add(Resource(this, "Wheat").setBasePrice(0.5).addTypes("farmable"));
		add(Resource(this, "Wood").setBasePrice(1.).addTypes("fuel", "natural", "centrifugable").add(Processor::Type::Furnace, {1., "Charcoal"}));
		add(Resource(this, "Yeast").setBasePrice(1.).addTypes("alive", "microorganism", "fermentable"));

		add(Resource(this, "Actinium").setBasePrice(29e12_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Aluminum").setBasePrice(1.79_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Antimony").setBasePrice(5.79_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Argon").setBasePrice(0.931_e).addTypes("element", "chemical", "gas"));
		add(Resource(this, "Arsenic").setBasePrice(1.15_e).addTypes("element", "chemical", "metal", "toxic"));
		add(Resource(this, "Barium").setBasePrice(0.2605_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Berkelium").setBasePrice(185e9_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Beryllium").setBasePrice(857._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Bismuth").setBasePrice(6.36_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Boron").setBasePrice(3.68_e).addTypes("element", "chemical", "metalloid"));
		add(Resource(this, "Bromine").setBasePrice(4.39_e).addTypes("element", "chemical", "liquid"));
		add(Resource(this, "Cadmium").setBasePrice(2.73_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Cesium").setBasePrice(61800._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Calcium").setBasePrice(2.28_e).addTypes("element", "chemical"));
		add(Resource(this, "Cerium").setBasePrice(4.64_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Chlorine").setBasePrice(0.082_e).addTypes("element", "chemical"));
		add(Resource(this, "Chromium").setBasePrice(9.4_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Cobalt").setBasePrice(32.8_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Dysprosium").setBasePrice(307._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Erbium").setBasePrice(26.4_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Europium").setBasePrice(31.4_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Gadolinium").setBasePrice(28.6_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Gallium").setBasePrice(148._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Germanium").setBasePrice(962._e).addTypes("element", "chemical", "metalloid"));
		add(Resource(this, "Gold").setBasePrice(44'800._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Hafnium").setBasePrice(900._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Helium").setBasePrice(24._e).addTypes("element", "chemical", "gas"));
		add(Resource(this, "Holmium").setBasePrice(57.1_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Indium").setBasePrice(167._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Iodine").setBasePrice(35._e).addTypes("element", "chemical"));
		add(Resource(this, "Krypton").setBasePrice(290._e).addTypes("element", "chemical", "gas"));
		add(Resource(this, "Lanthanum").setBasePrice(4.85_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Lead").setBasePrice(2._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Lutetium").setBasePrice(643._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Mercury").setBasePrice(30.2_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Molybdenum").setBasePrice(40.1_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Neodymium").setBasePrice(57.5_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Neon").setBasePrice(240._e).addTypes("element", "chemical", "gas"));
		add(Resource(this, "Neptunium").setBasePrice(660000._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Nickel").setBasePrice(13.9_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Niobium").setBasePrice(73.5_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Nitrogen").setBasePrice(0.14_e).addTypes("element", "chemical", "gas"));
		add(Resource(this, "Osmium").setBasePrice(12000._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Palladium").setBasePrice(49500._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Phosphorus").setBasePrice(2.69_e).addTypes("element", "chemical"));
		add(Resource(this, "Platinum").setBasePrice(27800._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Plutonium").setBasePrice(6490000._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Polonium").setBasePrice(49.2e12_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Praseodymium").setBasePrice(103._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Promethium").setBasePrice(460'000._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Rhenium").setBasePrice(3580._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Rhodium").setBasePrice(147'000._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Ruthenium").setBasePrice(10'500._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Samarium").setBasePrice(13.9_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Scandium").setBasePrice(3460._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Selenium").setBasePrice(21.4_e).addTypes("element", "chemical"));
		add(Resource(this, "Silver").setBasePrice(521._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Sodium").setBasePrice(3._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Strontium").setBasePrice(6.605_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Sulfur").setBasePrice(0.0926_e).addTypes("element", "chemical"));
		add(Resource(this, "Tantalum").setBasePrice(305._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Technetium").setBasePrice(1e5_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Calcium").setBasePrice(2.28_e).addTypes("element", "chemical"));
		add(Resource(this, "Carbon").setBasePrice(0.122_e).addTypes("element", "chemical"));
		add(Resource(this, "Copper").setBasePrice(6._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Fluorine").setBasePrice(2._e).addTypes("element", "chemical"));
		add(Resource(this, "Hydrogen").setBasePrice(1.39_e).addTypes("element", "chemical"));
		add(Resource(this, "Iron").setBasePrice(ELEMENT_BASE).addTypes("element", "chemical", "metal", "iron"));
		add(Resource(this, "Lithium").setBasePrice(83.5_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Rubidium").setBasePrice(15'500._e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Magnesium").setBasePrice(2.32_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Manganese").setBasePrice(1.82_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Oxygen").setBasePrice(0.154_e).addTypes("element", "chemical", "gas"));
		add(Resource(this, "Potassium").setBasePrice(12.85_e).addTypes("element", "chemical", "metal"));
		add(Resource(this, "Silicon").setBasePrice(1.7_e).addTypes("element", "chemical", "metalloid"));

		add(Resource(this, "Ammolite").setBasePrice(21.).addTypes("mineral", "electrolyzable").setDescription("CaCO₃"));
		add(Resource(this, "Azurite").setBasePrice(158.).addTypes("mineral", "electrolyzable").setDescription("Cu₃(CO₃)₂(OH)₂"));
		add(Resource(this, "Chrysoberyl").setBasePrice(6094.).addTypes("mineral", "electrolyzable").setDescription("BeAl₂O₄"));
		add(Resource(this, "Fluorite").setBasePrice(45.).addTypes("mineral", "electrolyzable").setDescription("CaF₂"));
		add(Resource(this, "Lepidolite").setBasePrice(440742.).addTypes("mineral", "electrolyzable").setDescription("K(Li,Al)₃(Al,Si,Rb)₄O₁₀(F,OH)₂"));
		add(Resource(this, "Malachite").setBasePrice(111.).addTypes("mineral", "electrolyzable").setDescription("Cu₂CO₃(OH)₂"));
		add(Resource(this, "Opal").setBasePrice(35.).addTypes("mineral", "electrolyzable").setDescription("SiO₂ + H₂O"));
		add(Resource(this, "Peridot").setBasePrice(56.).addTypes("mineral", "electrolyzable").setDescription("(Mg,Fe)₂SiO₄"));
		add(Resource(this, "Pyrope").setBasePrice(124.).addTypes("mineral", "electrolyzable").setDescription("Mg₃Al₂(SiO₄)₃"));
		add(Resource(this, "Rhodochrosite").setBasePrice(18.).addTypes("mineral", "electrolyzable").setDescription("MnCO₃"));

		add(Resource(this, "Glucose").setBasePrice({{6, "Carbon"}, {12, "Hydrogen"}, {6, "Oxygen"}}).addTypes("food"));
	}

	void Game::add(Processor::Type type, const Resource::Map &cost) {
		processorCosts.emplace(type, cost);
	}

	void Game::addProcessorCosts() {
		add(Processor::Type::Furnace,       {{"Stone", 100.}});
		add(Processor::Type::Centrifuge,    {{"Iron",  200.}});
		add(Processor::Type::Fermenter,     {{"Glass", 100.}});
		add(Processor::Type::Crusher,       {{"Iron",  300.}});
		add(Processor::Type::Refinery,      {{"Iron",  500.}, {"Microchip", 2.}});
		add(Processor::Type::RocketFurnace, {{"Iron",  500.}, {"Plastic", 500.}, {"Microchip", 10.}});
		add(Processor::Type::Electrolyzer,  {{"Iron",  500.}, {"Glass", 500.}});
	}

	void Game::addAll() {
		if (!ready) {
			addResources();
			addProcessorCosts();
			ready = true;
		}
	}

	std::string Game::randomResource(const Resource::Type &type) const {
		std::vector<std::string> choices;
		choices.reserve(resources.size());
		for (const auto &[name, resource]: resources) {
			if (resource.types.count(type) != 0)
				choices.push_back(name);
		}
		if (choices.empty())
			return "";
		return choices.at(randomRange(0ul, choices.size() - 1ul));
	}

	bool Game::updatePosition(Region &region, const Position &new_position) {
		if (regions.count(region.position) == 0)
			return false;
		auto handler = regions.extract(region.position);
		handler.mapped()->position = new_position;
		if (&region != handler.mapped().get())
			region.position = new_position;
		handler.key() = new_position;
		regions.insert(std::move(handler));
		return true;
	}

	Position Game::suggestPosition(bool is_free, s64 x, s64 y) {
		enum Direction {Up, Right, Down, Left};
		Direction direction = Up;
		int max_extent = 1;
		int extent = 0;
		for (;;) {
			if (regions.count({x, y}) == (is_free? 0 : 1))
				return {x, y};

			switch (direction) {
				case Up:    --y; break;
				case Right: ++x; break;
				case Down:  ++y; break;
				case Left:  --x; break;
			}

			if (++extent == max_extent) {
				extent = 0;
				switch (direction) {
					case Up:    direction = Right; break;
					case Right: direction = Down; ++max_extent; break;
					case Down:  direction = Left;  break;
					case Left:  direction = Up;   ++max_extent; break;
				}
			}
		}
	}

	bool Game::updateName(Region &region, const std::string &new_name) {
		if (new_name.find_first_of(Region::INVALID_CHARS) != std::string::npos)
			throw std::invalid_argument("Invalid region name.");
		region.name = new_name;
		return true;
	}

	Region & Game::currentRegion() {
		return *regions.at(position);
	}

	std::shared_ptr<Region> Game::currentRegionPointer() {
		if (regions.count(position) == 0)
			return nullptr;
		return regions.at(position);
	}

	bool Game::erase(Region &region) {
		// If this region is the only region, don't erase it.
		if (regions.size() == 1 && regions.begin()->second->position == region.position)
			return false;
		const bool reposition = region.position == position;
		for (auto iter = extractions.begin(); iter != extractions.end();) {
			const Extraction &extraction = *iter;
			if (extraction.area->parent->position == region.position)
				extractions.erase(iter++);
			else
				++iter;
		}
		const bool out = regions.erase(region.position) != 0;
		if (reposition)
			position = suggestPosition(false, position.x, position.y);
		return out;
	}

	void Game::eraseExtractions(const Area &area) {
		for (auto iter = extractions.begin(); iter != extractions.end();) {
			const Extraction &extraction = *iter;
			if (extraction.area->name == area.name && extraction.area->parent == area.parent)
				extractions.erase(iter++);
			else
				++iter;
		}
	}

	void Game::setMoney(size_t new_money) {
		if (money != new_money) {
			money = new_money;
			window.moneyDispatcher.emit();
		}
	}

	void Game::addToInventory(const std::string &resource_name, double amount) {
		inventory[resource_name] += amount;
	}

	double Game::adjust(double to_extract, double in_region, const Extraction &extraction) {
		// Questionable.
		if (ltna(in_region - to_extract, extraction.minimum))
			return std::max(0., in_region - extraction.minimum);
		return to_extract;
	}

	void Game::tick(double delta) {
		for (auto &pair: regions)
			pair.second->tick(delta);

		for (auto iter = extractions.begin(); iter != extractions.end();) {
			Extraction &extraction = *iter;
			double &in_inventory = inventory[extraction.resourceName];
			double &in_region = extraction.area->resources[extraction.resourceName];
			const double base = extraction.rate * delta;
			if (extraction.startAmount == 0) { // Eternal extraction
				const double to_extract = adjust(std::min(base, in_region), in_region, extraction);
				if (lte(to_extract, in_region)) {
					in_region -= to_extract;
					in_inventory += to_extract;
				}

				++iter;
			} else {
				const double to_extract = adjust(std::min(base, extraction.amount), in_region, extraction);
				if (lte(in_region, to_extract)) {
					in_inventory += in_region;
					const double floored = std::floor(in_inventory);
					if (1 - (in_inventory - floored) < Resource::MIN_AMOUNT)
						in_inventory = floored + 1;
					extraction.area->resources.erase(extraction.resourceName);
					extractions.erase(iter++);
				} else {
					in_inventory += to_extract;
					in_region -= to_extract;
					extraction.amount -= to_extract;
					if (extraction.amount < Resource::MIN_AMOUNT) {
						const double floored = std::floor(in_inventory);
						if (1 - (in_inventory - floored) < Resource::MIN_AMOUNT)
							in_inventory = floored + 1;
						extractions.erase(iter++);
					} else
						++iter;
				}
			}
		}

		for (auto &processor: processors)
			processor->tick(delta);

		if (automationEnabled)
			for (auto &link: automationLinks)
				link.tick();
	}

	std::shared_ptr<Game> Game::loadDefaults(AppWindow &window) {
		std::shared_ptr<Game> game = std::make_shared<Game>(window, "");
		game->regions.clear();
		Region &home = *game->regions.insert({Position(0, 0), std::make_unique<Region>(*game, NameGen::makeRandomLanguage().makeName(), Position(0, 0), 128)}).first->second;
		home.greed = 0.25;
		home.money = 10'000;
		auto forest = std::make_shared<ForestArea>(&home, 32);
		auto housing = std::make_shared<HousingArea>(&home, 20);
		auto mountain = std::make_shared<MountainArea>(&home, 48);
		auto lake = std::make_shared<LakeArea>(&home, 8);
		auto farmland = std::make_shared<FarmlandArea>(&home, 20);
		forest->setName("Forest").setPlayerOwned(true);
		housing->setName("Small Town");
		mountain->setName("Mountain").setPlayerOwned(true);
		lake->setName("Lake").setPlayerOwned(true);
		farmland->setName("Farm").setPlayerOwned(false);
		home += forest;
		home += housing;
		home += mountain;
		home += lake;
		home += farmland;
		auto furnace = std::make_shared<Furnace>(*game);
		furnace->setName("Furnace");
		game->processors.push_back(furnace);
		Logger::info("Loaded default data.");
		return game;
	}

	Extraction & Game::extract(Area &area, const std::string &name, double amount, double minimum) {
		return extractions.emplace_back(&area, name, amount, amount, resources.at(name).defaultExtractionRate, minimum);
	}

	Extraction & Game::extract(Area &area, const std::string &name, double amount, double minimum, double rate) {
		return extractions.emplace_back(&area, name, amount, amount, rate, minimum);
	}

	std::string Game::toString() const {
		std::stringstream out;
		out.imbue(std::locale::classic());
		out << "[Regions]\n";
		for (const auto &pair: regions)
			out << pair.second->toString() << "\n";
		out << "\n[Inventory]\n";
		out << "money=" << money << "\n";
		for (const auto &[name, amount]: inventory)
			out << name << "=" << amount << "\n";
		out << "\n[CraftingInventory]\n";
		for (const auto &[name, amount]: craftingInventory)
			out << name << "=" << amount << "\n";
		out << "\n[Position]\n";
		out << position.x << "," << position.y << "\n";
		out << "\n[Extractions]\n";
		for (const Extraction &extraction: extractions)
			out << extraction.toString() << "\n";
		out << "\n[Processors]\n";
		for (const std::shared_ptr<Processor> &processor: processors)
			out << processor->toString() << "\n";
		out << "\n[Automations]\n";
		for (const AutomationLink &link: automationLinks)
			out << link.toString() << "\n";
		return out.str();
	}

	std::shared_ptr<Game> Game::fromString(AppWindow &window, const std::string &str, const std::string &path) {
		std::vector<std::string> lines = split(str, "\n", true);
		/** Compliant saves must follow this order beginning at Regions. */
		enum class Mode {None, Regions, Inventory, CraftingInventory, Position, Extractions, Processors, Automations};
		Mode mode = Mode::None;

		std::shared_ptr<Game> out = std::make_shared<Game>(window, path);

		for (std::string &line: lines) {
			if (line.empty() || line == "\r")
				continue;
			if (line.back() == '\r')
				line.pop_back();
			if (line[0] == '[') {
				if (line == "[Regions]")
					mode = Mode::Regions;
				else if (line == "[Inventory]")
					mode = Mode::Inventory;
				else if (line == "[CraftingInventory]")
					mode = Mode::CraftingInventory;
				else if (line == "[Position]")
					mode = Mode::Position;
				else if (line == "[Extractions]")
					mode = Mode::Extractions;
				else if (line == "[Processors]")
					mode = Mode::Processors;
				else if (line == "[Automations]")
					mode = Mode::Automations;
				else {
					Logger::error("Invalid line: \"%s\"", line.c_str());
					for (char ch: line)
						Logger::info("%d / '%c'", ch, ch);
					throw std::invalid_argument("Invalid line");
				}
			} else {
				switch (mode) {
					case Mode::Regions: {
						auto region = Region::fromString(*out, line);
						out->regions.emplace(region->position, std::move(region));
						break;
					}
					case Mode::Inventory:
					case Mode::CraftingInventory: {
						const size_t equals = line.find('=');
						if (equals == std::string::npos)
							throw std::invalid_argument("Invalid Inventory line");
						const std::string type = line.substr(0, equals);
						const double amount = parseDouble(line.substr(equals + 1));
						if (type == "money")
							out->money = amount;
						else
							(mode == Mode::CraftingInventory? out->craftingInventory : out->inventory).emplace(type, amount);
						break;
					}
					case Mode::Position: {
						const size_t comma = line.find(',');
						if (comma == std::string::npos)
							throw std::invalid_argument("Invalid Position line");
						out->position = {parseLong(line.substr(0, comma)), parseLong(line.substr(comma + 1))};
						break;
					}
					case Mode::Extractions:
						out->extractions.push_back(Extraction::fromString(*out, line));
						break;
					case Mode::Processors: {
						std::shared_ptr<Processor> processor(Processor::fromString(*out, line));
						out->processors.push_back(processor);
						out->processorsByID.emplace(processor->id, processor);
						break;
					}
					case Mode::Automations:
						out->automationLinks.emplace_back(*out, line);
						out->automationLinks.back().setup();
						break;
					default: throw std::runtime_error("Invalid mode: " + std::to_string(static_cast<int>(mode)));
				}
			}
		}

		return out;
	}

	std::shared_ptr<Game> Game::load(AppWindow &window, const std::string &path) {
		if (!FS::fileExists(path))
			throw std::runtime_error("Save data doesn't exist");
		return fromString(window, FS::readFile(path));
	}

	void Game::save() {
		FS::writeFile(path, toString());
	}

	Game & Game::operator+=(std::unique_ptr<Region> &&ptr) {
		if (regions.count(ptr->position) != 0)
			throw std::runtime_error("A region already exists at " + std::string(ptr->position));
		regions.emplace(ptr->position, std::move(ptr));
		return *this;
	}

	Extraction * Game::getExtraction(const Area &area, const std::string &name) {
		for (Extraction &extraction: extractions)
			if (extraction.area == &area && extraction.resourceName == name)
				return &extraction;
		return nullptr;
	}

	Extraction * Game::getExtraction(const Area &area, const std::string &name,
	                                 decltype(extractions)::iterator &iter_out) {
		for (auto iter = extractions.begin(), end = extractions.end(); iter != end; ++iter)
			if (iter->area == &area && iter->resourceName == name) {
				iter_out = iter;
				return &*iter;
			}
		iter_out = extractions.end();
		return nullptr;
	}

	const Extraction * Game::getExtraction(const Area &area, const std::string &name) const {
		for (const Extraction &extraction: extractions)
			if (extraction.area == &area && extraction.resourceName == name)
				return &extraction;
		return nullptr;
	}
}
