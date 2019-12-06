#pragma once

#include "../type/list_type.h"

#include "assignment.h"

namespace cminus::evaluator{
	class list : public object, public assignment{
	public:
		virtual ~list();

		virtual id_type get_id() const override;

		virtual memory_ptr_type evaluate_unary_left(operators::id op, memory_ptr_type target) const override;

		virtual memory_ptr_type evaluate_unary_right(operators::id op, memory_ptr_type target) const override;

	protected:
		virtual memory_ptr_type evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const override;
	};
}
