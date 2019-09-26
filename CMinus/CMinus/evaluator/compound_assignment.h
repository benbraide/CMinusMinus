#pragma once

#include "../type/primitive_type.h"

#include "evaluator_object.h"

namespace cminus::evaluator{
	class compound_assignment{
	public:
		struct optimized_info{
			type::number_primitive *type;
			object::memory_ptr_type mem;
		};

		virtual ~compound_assignment();

		virtual bool assign(operators::id op, object::memory_ptr_type left_value, object::node_ptr_type right) const;

	protected:
		template <typename target_type>
		bool evaluate_and_assign_(operators::id op, const optimized_info &target, const optimized_info &value) const{
			if (target.type->is_nan(*target.mem))//Expression evaluates to NaN
				return true;

			if (value.type->is_nan(*value.mem)){//Expression evaluates to NaN
				target.mem->write_scalar(type::get_nan<target_type>::template value());
				return true;
			}

			auto right_value = target_type();
			switch (op){
			case operators::id::compound_plus:
				target.mem->write_scalar(target.mem->read_scalar<target_type>() + value.mem->read_scalar<target_type>());
				return true;
			case operators::id::compound_minus:
				target.mem->write_scalar(target.mem->read_scalar<target_type>() - value.mem->read_scalar<target_type>());
				return true;
			case operators::id::compound_times:
				target.mem->write_scalar(target.mem->read_scalar<target_type>() * value.mem->read_scalar<target_type>());
				return true;
			case operators::id::compound_divide:
				if ((right_value = value.mem->read_scalar<target_type>()) == static_cast<target_type>(0))
					target.mem->write_scalar(type::get_nan<target_type>::template value());
				else
					target.mem->write_scalar(target.mem->read_scalar<target_type>() / value.mem->read_scalar<target_type>());
				return true;
			default:
				break;
			}

			return false;
		}

		template <typename target_type>
		bool evaluate_and_assign_integral_(operators::id op, const optimized_info &target, const optimized_info &value) const{
			if (evaluate_and_assign_<target_type>(op, target, value))
				return true;

			auto right_value = target_type();
			switch (op){
			case operators::id::compound_bitwise_or:
				target.mem->write_scalar(target.mem->read_scalar<target_type>() | value.mem->read_scalar<target_type>());
				return true;
			case operators::id::compound_bitwise_xor:
				target.mem->write_scalar(target.mem->read_scalar<target_type>() ^ value.mem->read_scalar<target_type>());
				return true;
			case operators::id::compound_bitwise_and:
				target.mem->write_scalar(target.mem->read_scalar<target_type>() & value.mem->read_scalar<target_type>());
				return true;
			case operators::id::compound_left_shift:
				target.mem->write_scalar(target.mem->read_scalar<target_type>() << value.mem->read_scalar<target_type>());
				return true;
			case operators::id::compound_right_shift:
				target.mem->write_scalar(target.mem->read_scalar<target_type>() >> value.mem->read_scalar<target_type>());
				return true;
			case operators::id::compound_modulus:
				if ((right_value = value.mem->read_scalar<target_type>()) == static_cast<target_type>(0))
					target.mem->write_scalar(type::get_nan<target_type>::template value());
				else
					target.mem->write_scalar(target.mem->read_scalar<target_type>() % value.mem->read_scalar<target_type>());
				return true;
			default:
				break;
			}

			return false;
		}
	};
}
