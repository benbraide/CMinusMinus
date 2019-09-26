#pragma once

#include "../type/type_object.h"
#include "../node/node_object.h"

#include "../operators/operator_id.h"
#include "../attribute/attribute_guard.h"

#include "evaluator_exception.h"

namespace cminus::evaluator{
	class object{
	public:
		using memory_ptr_type = std::shared_ptr<memory::reference>;
		using node_ptr_type = std::shared_ptr<node::object>;

		virtual ~object();

		virtual memory_ptr_type evaluate_unary_left(operators::id op, memory_ptr_type target) const = 0;

		virtual memory_ptr_type evaluate_unary_right(operators::id op, memory_ptr_type target) const = 0;

		virtual memory_ptr_type evaluate_binary(operators::id op, memory_ptr_type left_value, node_ptr_type right) const = 0;

		virtual memory_ptr_type evaluate_binary(operators::id op, memory_ptr_type left_value, memory_ptr_type right_value) const;
	};
}
