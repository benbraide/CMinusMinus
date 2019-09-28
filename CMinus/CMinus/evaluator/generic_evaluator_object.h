#pragma once

#include "comparison.h"
#include "assignment.h"

namespace cminus::evaluator{
	template <class target_type>
	class generic_object : public object, public assignment, public generic_comparison<target_type>{
	public:
		virtual ~generic_object() = default;

		virtual memory_ptr_type evaluate_unary_left(operators::id op, memory_ptr_type target) const override{
			throw exception::unsupported_op();
			return nullptr;
		}

		virtual memory_ptr_type evaluate_unary_right(operators::id op, memory_ptr_type target) const override{
			throw exception::unsupported_op();
			return nullptr;
		}

	protected:
		virtual memory_ptr_type evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const override{
			if (assignment::assign(op, left_value, right))
				return left_value;

			if (auto result = generic_comparison<target_type>::template evaluate(op, left_value, right); result != nullptr)
				return result;

			throw exception::unsupported_op();
			return nullptr;
		}
	};
}
