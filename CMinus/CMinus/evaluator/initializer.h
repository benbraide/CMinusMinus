#pragma once

#include "../type/type_object.h"
#include "../memory/memory_reference.h"
#include "../attribute/attribute_guard.h"

#include "evaluator_exception.h"

namespace cminus::evaluator{
	class initializer{
	public:
		virtual ~initializer();

		virtual void initialize(std::shared_ptr<memory::reference> target, std::shared_ptr<memory::reference> value) const = 0;
	};
}
