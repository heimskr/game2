#include "Processor.h"

namespace Game2 {
	struct Electrolyzer: public Processor {
		using Processor::Processor;
		double tick(double delta) override;
		Type getType() const override { return Type::Electrolyzer; }
	};
}
