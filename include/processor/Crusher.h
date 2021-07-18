#include "Processor.h"

namespace Game2 {
	struct Crusher: public Processor {
		using Processor::Processor;
		double tick(double delta) override;
		Type getType() const override { return Type::Crusher; }
	};
}
