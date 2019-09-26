#pragma once

#include "../type/type_object.h"
#include "../node/node_object.h"

#include "../operators/operator_id.h"
#include "../attribute/attribute_guard.h"

#include "evaluator_exception.h"

namespace cminus::evaluator{
	class explicit_comparison{
	public:
		using memory_ptr_type = std::shared_ptr<memory::reference>;
		using node_ptr_type = std::shared_ptr<node::object>;

		virtual ~explicit_comparison();

		virtual memory_ptr_type evaluate(operators::id op, memory_ptr_type left_value, node_ptr_type right) const;
	};
}
