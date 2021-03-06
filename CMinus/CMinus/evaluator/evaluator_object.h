#pragma once

#include "explicit_comparison.h"

namespace cminus::evaluator{
	class object : public explicit_comparison{
	public:
		enum class id_type{
			nil,
			undefined,
			boolean,
			byte,
			number,
			function,
			pointer,
			array_,
			class_,
			string,
			enum_,
			iter,
			riter,
			type,
		};

		virtual ~object();

		virtual id_type get_id() const = 0;

		virtual memory_ptr_type evaluate_unary_left(operators::id op, memory_ptr_type target) const = 0;

		virtual memory_ptr_type evaluate_unary_right(operators::id op, memory_ptr_type target) const = 0;

		virtual memory_ptr_type evaluate_binary(operators::id op, memory_ptr_type left_value, node_ptr_type right) const;

		virtual memory_ptr_type evaluate_binary(operators::id op, memory_ptr_type left_value, memory_ptr_type right_value) const;

	protected:
		virtual memory_ptr_type evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const = 0;
	};

	class noop : public object{
	public:
		virtual ~noop();

		virtual id_type get_id() const override;

		virtual memory_ptr_type evaluate_unary_left(operators::id op, memory_ptr_type target) const override;

		virtual memory_ptr_type evaluate_unary_right(operators::id op, memory_ptr_type target) const override;

	protected:
		virtual memory_ptr_type evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const override;
	};
}
