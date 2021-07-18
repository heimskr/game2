#pragma once

#include "Area.h"

namespace Game2 {
	class EmptyArea: public Area {
		public:
			EmptyArea(Region *, size_t = 0);
			Type getType() const override { return Type::Empty; }
			std::string description() const override;
			void tick(double delta) override;
	};
}
