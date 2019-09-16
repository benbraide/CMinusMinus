#pragma once

#include "../type/type_object.h"
#include "../memory/memory_reference.h"

#include "evaluator_exception.h"

namespace cminus::evaluator{
	class object{
	public:
		virtual ~object() = default;
	};
}
