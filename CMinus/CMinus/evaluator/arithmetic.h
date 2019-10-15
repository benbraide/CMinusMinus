#pragma once

#include "../type/primitive_type.h"

#include "evaluator_object.h"

namespace cminus::evaluator{
	class arithmetic{
	public:
		virtual ~arithmetic();

		virtual object::memory_ptr_type evaluate(operators::id op, object::memory_ptr_type target, bool is_left) const;

		virtual object::memory_ptr_type evaluate(operators::id op, object::memory_ptr_type left_value, object::node_ptr_type right) const;

	protected:
		template <typename target_type>
		object::memory_ptr_type evaluate_unsigned_(operators::id op, object::memory_ptr_type target, bool is_left) const{
			return ((is_left && op == operators::id::plus) ? std::make_shared<memory::scalar_reference<target_type>>(target->get_type(), target->read_scalar<target_type>()) : nullptr);
		}

		template <typename target_type>
		object::memory_ptr_type evaluate_(operators::id op, object::memory_ptr_type target, bool is_left) const{
			if (auto result = evaluate_unsigned_<target_type>(op, target, is_left); result != nullptr)
				return result;

			if (!is_left ||  op != operators::id::minus)
				return nullptr;

			if (target->is_nan())//Expression evaluates to NaN
				return std::make_shared<memory::scalar_reference<target_type>>(target->get_type(), type::get_nan<target_type>::template value());

			return std::make_shared<memory::scalar_reference<target_type>>(target->get_type(), -target->read_scalar<target_type>());
		}

		template <typename target_type>
		object::memory_ptr_type evaluate_integral_(operators::id op, object::memory_ptr_type target, bool is_left, target_type multiplier = static_cast<target_type>(1)) const{
			auto is_bitwise_inverse = (op == operators::id::bitwise_inverse);
			if (!is_bitwise_inverse && op != operators::id::increment && op != operators::id::decrement)
				return nullptr;

			if (target->is_nan()){//Expression evaluates to NaN
				if (is_left && !is_bitwise_inverse)
					return target;
				return std::make_shared<memory::scalar_reference<target_type>>(target->get_type(), type::get_nan<target_type>::template value());
			}

			if (is_bitwise_inverse)
				return std::make_shared<memory::scalar_reference<target_type>>(target->get_type(), ~target->read_scalar<target_type>());

			target_type old_value = target->read_scalar<target_type>(), value;
			if (op == operators::id::increment)
				value = (old_value + multiplier);
			else//Decrement
				value = (old_value - multiplier);

			target->write(std::make_shared<memory::scalar_reference<target_type>>(target->get_type(), value));
			if (is_left)
				return target;

			return std::make_shared<memory::scalar_reference<target_type>>(target->get_type(), old_value);
		}

		template <typename target_type>
		object::memory_ptr_type evaluate_(operators::id op, object::memory_ptr_type left, object::memory_ptr_type right, target_type multiplier = static_cast<target_type>(1)) const{
			if (left->is_nan() || right->is_nan())//Expression evaluates to NaN
				return std::make_shared<memory::scalar_reference<target_type>>(left->get_type(), type::get_nan<target_type>::template value());

			auto right_value = target_type();
			switch (op){
			case operators::id::plus:
				return std::make_shared<memory::scalar_reference<target_type>>(left->get_type(), (left->read_scalar<target_type>() + (right->read_scalar<target_type>() * multiplier)));
			case operators::id::minus:
				return std::make_shared<memory::scalar_reference<target_type>>(left->get_type(), (left->read_scalar<target_type>() - (right->read_scalar<target_type>() * multiplier)));
			case operators::id::times:
				return std::make_shared<memory::scalar_reference<target_type>>(left->get_type(), (left->read_scalar<target_type>() * right->read_scalar<target_type>()));
			case operators::id::divide:
				if ((right_value = right->read_scalar<target_type>()) == static_cast<target_type>(0))
					return std::make_shared<memory::scalar_reference<target_type>>(left->get_type(), type::get_nan<target_type>::template value());
				return std::make_shared<memory::scalar_reference<target_type>>(left->get_type(), (left->read_scalar<target_type>() / right_value));
			default:
				break;
			}

			return nullptr;
		}

		template <typename target_type>
		object::memory_ptr_type evaluate_integral_(operators::id op, object::memory_ptr_type left, object::memory_ptr_type right, target_type multiplier = static_cast<target_type>(1)) const{
			if (op == operators::id::index){
				auto index = static_cast<std::size_t>(right->read_scalar<target_type>());
				if (sizeof(target_type) <= index)
					throw runtime::exception::out_of_range();

				if (left->is_lvalue())
					return create_byte_ref_((left->get_address() + index), left->is_const());

				return create_byte_val_(runtime::object::memory_object->read_scalar<std::byte>(left->get_address() + index));
			}

			if (auto result = evaluate_<target_type>(op, left, right, multiplier); result != nullptr)
				return result;

			auto right_value = target_type();
			switch (op){
			case operators::id::bitwise_or:
				return std::make_shared<memory::scalar_reference<target_type>>(left->get_type(), (left->read_scalar<target_type>() | right->read_scalar<target_type>()));
			case operators::id::bitwise_xor:
				return std::make_shared<memory::scalar_reference<target_type>>(left->get_type(), (left->read_scalar<target_type>() ^ right->read_scalar<target_type>()));
			case operators::id::bitwise_and:
				return std::make_shared<memory::scalar_reference<target_type>>(left->get_type(), (left->read_scalar<target_type>() & right->read_scalar<target_type>()));
			case operators::id::left_shift:
				return std::make_shared<memory::scalar_reference<target_type>>(left->get_type(), (left->read_scalar<target_type>() << right->read_scalar<target_type>()));
			case operators::id::right_shift:
				return std::make_shared<memory::scalar_reference<target_type>>(left->get_type(), (left->read_scalar<target_type>() >> right->read_scalar<target_type>()));
			case operators::id::modulus:
				if ((right_value = right->read_scalar<target_type>()) == static_cast<target_type>(0))
					return std::make_shared<memory::scalar_reference<target_type>>(left->get_type(), type::get_nan<target_type>::template value());
				return std::make_shared<memory::scalar_reference<target_type>>(left->get_type(), (left->read_scalar<target_type>() % right_value));
			default:
				break;
			}

			return nullptr;
		}

		virtual object::memory_ptr_type create_byte_val_(std::byte value) const;

		virtual object::memory_ptr_type create_byte_ref_(std::size_t address, bool is_const) const;
	};
}
