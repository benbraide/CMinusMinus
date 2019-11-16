#pragma once

#include "../declaration/callable_declaration_group.h"

#include "evaluator_object.h"

namespace cminus::evaluator{
	class class_ : public object{
	public:
		virtual ~class_();

		virtual id_type get_id() const override;

		virtual memory_ptr_type evaluate_unary_left(operators::id op, memory_ptr_type target) const override;

		virtual memory_ptr_type evaluate_unary_right(operators::id op, memory_ptr_type target) const override;

	protected:
		virtual memory_ptr_type evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const override;

		virtual declaration::callable_group *find_operator_(operators::id op, std::shared_ptr<type::object> target_type) const;
	};
}
