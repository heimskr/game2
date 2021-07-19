#pragma once

#include <array>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>

namespace Game2 {
	class Context;
	class Game;

	class Processor {
		public:
			enum class Type {Furnace, Centrifuge, Fermenter, Crusher, Refinery, RocketFurnace, Electrolyzer};
			static constexpr const char *INVALID_CHARS = ":;/";
			static constexpr auto TYPES = std::array {
				Type::Centrifuge,
				Type::Crusher,
				Type::Electrolyzer,
				Type::Fermenter,
				Type::Furnace,
				Type::Refinery,
				Type::RocketFurnace,
			};

			Game *game;
			std::map<std::string, double> input, output;
			/** Resources in this set won't be auto-extracted. */
			std::unordered_set<std::string> frozen;
			bool autoExtract = false;
			std::string name, id;

			Processor(Game &);

			virtual ~Processor() {}

			Processor & setInput(const std::map<std::string, double> &);
			Processor & setOutput(const std::map<std::string, double> &);
			Processor & setInput(std::map<std::string, double> &&);
			Processor & setOutput(std::map<std::string, double> &&);
			Processor & setAutoExtract(bool);
			Processor & setName(const std::string &);
			Processor & setName(std::string &&);
			Processor & setID(const std::string &);
			Processor & setID(std::string &&);

			virtual std::string toString() const;
			/** Returns the amount processed. */
			virtual double tick(double delta);
			void moveOutput();
			virtual Type getType() const = 0;
			static Processor * fromString(Game &, const std::string &);
			static Processor * ofType(Game &, Processor::Type);
			static const char * typeName(Type);
	};
}
