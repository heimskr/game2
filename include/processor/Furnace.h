#include "Processor.h"

namespace Game2 {
	class Furnace: public Processor {
		public:
			double fuel = 0.;

			using Processor::Processor;

			Furnace & setFuel(double);

			double tick(double delta) override;
			Type getType() const override { return Type::Furnace; }

			Processor * absorb(const nlohmann::json &) override;
			nlohmann::json toJSON() const override;
	};
}
