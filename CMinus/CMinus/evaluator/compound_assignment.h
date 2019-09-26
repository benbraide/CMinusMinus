#pragma once

#include "../type/primitive_type.h"

#include "evaluator_object.h"

namespace cminus::evaluator{
	class compound_assignment{
	public:
		virtual ~compound_assignment();

		virtual bool assign(operators::id op, object::memory_ptr_type left_value, object::node_ptr_type right) const;

	protected:
		template <typename target_type>
		bool evaluate_and_assign_(operators::id op, object::memory_ptr_type target, object::memory_ptr_type value) const{
			if (target->is_nan())//Expression evaluates to NaN
				return true;

			if (value->is_nan()){//Expression evaluates to NaN
				target->write_scalar(type::get_nan<target_type>::template value());
				return true;
			}

			auto computed_value = target_type();
			switch (op){
			case operators::id::compound_plus:
				target->write_scalar(target->read_scalar<target_type>() + value->read_scalar<target_type>());
				return true;
			case operators::id::compound_minus:
				target->write_scalar(target->read_scalar<target_type>() - value->read_scalar<target_type>());
				return true;
			case operators::id::compound_times:
				target->write_scalar(target->read_scalar<target_type>() * value->read_scalar<target_type>());
				return true;
			case operators::id::compound_divide:
				if ((computed_value = value->read_scalar<target_type>()) == static_cast<target_type>(0))
					target->write_scalar(type::get_nan<target_type>::template value());
				else
					target->write_scalar(target->read_scalar<target_type>() / computed_value);
				return true;
			default:
				break;
			}

			return false;
		}

		template <typename target_type>
		bool evaluate_and_assign_integral_(operators::id op, object::memory_ptr_type target, object::memory_ptr_type value) const{
			if (evaluate_and_assign_<target_type>(op, target, value))
				return true;

			auto computed_value = target_type();
			switch (op){
			case operators::id::compound_bitwise_or:
				target->write_scalar(target->read_scalar<target_type>() | value->read_scalar<target_type>());
				return true;
			case operators::id::compound_bitwise_xor:
				target->write_scalar(target->read_scalar<target_type>() ^ value->read_scalar<target_type>());
				return true;
			case operators::id::compound_bitwise_and:
				target->write_scalar(target->read_scalar<target_type>() & value->read_scalar<target_type>());
				return true;
			case operators::id::compound_left_shift:
				target->write_scalar(target->read_scalar<target_type>() << value->read_scalar<target_type>());
				return true;
			case operators::id::compound_right_shift:
				target->write_scalar(target->read_scalar<target_type>() >> value->read_scalar<target_type>());
				return true;
			case operators::id::compound_modulus:
				if ((computed_value = value->read_scalar<target_type>()) == static_cast<target_type>(0))
					target->write_scalar(type::get_nan<target_type>::template value());
				else
					target->write_scalar(target->read_scalar<target_type>() % computed_value);
				return true;
			default:
				break;
			}

			return false;
		}
	};
}
