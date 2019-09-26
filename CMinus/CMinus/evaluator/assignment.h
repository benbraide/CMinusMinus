#pragma once

#include "evaluator_object.h"

namespace cminus::evaluator{
	class assignment{
	public:
		virtual ~assignment();

		virtual bool assign(operators::id op, object::memory_ptr_type left_value, object::node_ptr_type right) const;
	};
}
