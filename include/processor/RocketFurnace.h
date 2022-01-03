#include "Processor.h"

namespace Game2 {
	class RocketFurnace: public Processor {
		public:
			double hydrogen = 0., oxygen = 0.;

			using Processor::Processor;

			RocketFurnace & setHydrogen(double);
			RocketFurnace & setOxygen(double);

			double tick(double delta) override;
			Type getType() const override { return Type::RocketFurnace; }

			Processor * absorb(const nlohmann::json &) override;
			nlohmann::json toJSON() const override;
	};
}
