#pragma once

#include "Area.h"

namespace Game2 {
	class ForestArea: public Area {
		public:
			ForestArea(Region *, size_t = 0);
			Type getType() const override { return Type::Forest; }
			std::string description() const override;
			void tick(double delta) override;
	};
}
