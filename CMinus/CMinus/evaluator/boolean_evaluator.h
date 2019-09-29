#pragma once

#include "../type/primitive_type.h"

#include "comparison.h"
#include "assignment.h"

namespace cminus::evaluator{
	class boolean : public object, public assignment, public generic_comparison<type::boolean_constant>{
	public:
		virtual ~boolean();

		virtual id_type get_id() const override;

		virtual memory_ptr_type evaluate_unary_left(operators::id op, memory_ptr_type target) const override;

		virtual memory_ptr_type evaluate_unary_right(operators::id op, memory_ptr_type target) const override;

	protected:
		virtual memory_ptr_type evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const override;
	};
}
