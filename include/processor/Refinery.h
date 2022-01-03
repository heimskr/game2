#include "Processor.h"
#include "defs/RefineryMode.h"

namespace Game2 {
	class Refinery: public Processor {
		public:
			RefineryMode mode = RefineryMode::Clarify;

			using Processor::Processor;

			Refinery & setMode(RefineryMode);

			double tick(double delta) override;
			Type getType() const override { return Type::Refinery; }

			Processor * absorb(const nlohmann::json &) override;
			nlohmann::json toJSON() const override;
	};
}
