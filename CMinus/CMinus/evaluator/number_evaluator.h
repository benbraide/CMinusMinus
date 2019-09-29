#pragma once

#include "comparison.h"
#include "assignment.h"
#include "arithmetic.h"
#include "compound_assignment.h"

namespace cminus::evaluator{
	class number : public object, public assignment, public compound_assignment, public arithmetic, public numeric_comparison{
	public:
		virtual ~number();

		virtual id_type get_id() const override;

		virtual memory_ptr_type evaluate_unary_left(operators::id op, memory_ptr_type target) const override;

		virtual memory_ptr_type evaluate_unary_right(operators::id op, memory_ptr_type target) const override;

	protected:
		virtual memory_ptr_type evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const override;
	};
}
