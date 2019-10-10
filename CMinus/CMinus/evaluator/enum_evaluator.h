#pragma once

#include "assignment.h"

namespace cminus::evaluator{
	class enum_ : public object, public assignment{
	public:
		virtual ~enum_();

		virtual id_type get_id() const override;

		virtual memory_ptr_type evaluate_unary_left(operators::id op, memory_ptr_type target) const override;

		virtual memory_ptr_type evaluate_unary_right(operators::id op, memory_ptr_type target) const override;

	protected:
		virtual memory_ptr_type evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const override;
	};
}
