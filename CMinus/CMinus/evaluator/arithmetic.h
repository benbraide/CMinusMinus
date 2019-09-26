#pragma once

#include "../type/primitive_type.h"

#include "evaluator_object.h"

namespace cminus::evaluator{
	class arithmetic{
	public:
		struct optimized_info{
			type::number_primitive *type;
			object::memory_ptr_type mem;
		};

		virtual ~arithmetic();

		virtual object::memory_ptr_type evaluate(operators::id op, object::memory_ptr_type left_value, object::node_ptr_type right) const;

	protected:
		template <typename target_type>
		object::memory_ptr_type evaluate_(operators::id op, const optimized_info &left, const optimized_info &right) const{
			if (left.type->is_nan(*left.mem) || right.type->is_nan(*right.mem))//Expression evaluates to NaN
				return std::make_shared<memory::scalar_reference<target_type>>(left.mem->get_type(), type::get_nan<target_type>::template value());

			auto right_value = target_type();
			switch (op){
			case operators::id::plus:
				return std::make_shared<memory::scalar_reference<target_type>>(left.mem->get_type(), (left.mem->read_scalar<target_type>() + right.mem->read_scalar<target_type>()));
			case operators::id::minus:
				return std::make_shared<memory::scalar_reference<target_type>>(left.mem->get_type(), (left.mem->read_scalar<target_type>() - right.mem->read_scalar<target_type>()));
			case operators::id::times:
				return std::make_shared<memory::scalar_reference<target_type>>(left.mem->get_type(), (left.mem->read_scalar<target_type>() * right.mem->read_scalar<target_type>()));
			case operators::id::divide:
				if ((right_value = right.mem->read_scalar<target_type>()) == static_cast<target_type>(0))
					return std::make_shared<memory::scalar_reference<target_type>>(left.mem->get_type(), type::get_nan<target_type>::template value());
				return std::make_shared<memory::scalar_reference<target_type>>(left.mem->get_type(), (left.mem->read_scalar<target_type>() / right.mem->read_scalar<target_type>()));
			default:
				break;
			}

			return nullptr;
		}

		template <typename target_type>
		object::memory_ptr_type evaluate_integral_(operators::id op, const optimized_info &left, const optimized_info &right) const{
			if (auto result = evaluate_<target_type>(op, left, right); result != nullptr)
				return result;

			auto right_value = target_type();
			switch (op){
			case operators::id::bitwise_or:
				return std::make_shared<memory::scalar_reference<target_type>>(left.mem->get_type(), (left.mem->read_scalar<target_type>() | right.mem->read_scalar<target_type>()));
			case operators::id::bitwise_xor:
				return std::make_shared<memory::scalar_reference<target_type>>(left.mem->get_type(), (left.mem->read_scalar<target_type>() ^ right.mem->read_scalar<target_type>()));
			case operators::id::bitwise_and:
				return std::make_shared<memory::scalar_reference<target_type>>(left.mem->get_type(), (left.mem->read_scalar<target_type>() & right.mem->read_scalar<target_type>()));
			case operators::id::left_shift:
				return std::make_shared<memory::scalar_reference<target_type>>(left.mem->get_type(), (left.mem->read_scalar<target_type>() << right.mem->read_scalar<target_type>()));
			case operators::id::right_shift:
				return std::make_shared<memory::scalar_reference<target_type>>(left.mem->get_type(), (left.mem->read_scalar<target_type>() >> right.mem->read_scalar<target_type>()));
			case operators::id::modulus:
				if ((right_value = right.mem->read_scalar<target_type>()) == static_cast<target_type>(0))
					return std::make_shared<memory::scalar_reference<target_type>>(left.mem->get_type(), type::get_nan<target_type>::template value());
				return std::make_shared<memory::scalar_reference<target_type>>(left.mem->get_type(), (left.mem->read_scalar<target_type>() % right.mem->read_scalar<target_type>()));
			default:
				break;
			}

			return nullptr;
		}
	};
}
