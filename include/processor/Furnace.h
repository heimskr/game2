#include "Processor.h"

namespace Game2 {
	class Furnace: public Processor {
		public:
			double fuel = 0.;

			using Processor::Processor;

			Furnace & setFuel(double);

			std::string toString() const override;
			double tick(double delta) override;
			Type getType() const override { return Type::Furnace; }
	};
}
