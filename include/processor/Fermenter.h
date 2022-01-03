#include "Processor.h"

namespace Game2 {
	class Fermenter: public Processor {
		public:
			double yeast = 0.;

			using Processor::Processor;

			Fermenter & setYeast(double);

			double tick(double delta) override;
			Type getType() const override { return Type::Fermenter; }

			Processor * absorb(const nlohmann::json &) override;
			nlohmann::json toJSON() const override;
	};
}
