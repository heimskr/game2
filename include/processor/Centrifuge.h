#include "Processor.h"

namespace Game2 {
	struct Centrifuge: public Processor {
		using Processor::Processor;
		double tick(double delta) override;
		Type getType() const override { return Type::Centrifuge; }
	};
}
