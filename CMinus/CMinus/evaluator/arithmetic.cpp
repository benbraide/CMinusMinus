#include "../type/string_type.h"
#include "../storage/global_storage.h"

#include "arithmetic.h"

cminus::evaluator::arithmetic::~arithmetic() = default;

cminus::evaluator::object::memory_ptr_type cminus::evaluator::arithmetic::evaluate(operators::id op, object::memory_ptr_type target, bool is_left) const{
	auto is_integral = false, is_lval = false;
	switch (op){
	case operators::id::plus:
	case operators::id::minus:
		if (!is_left)
			return nullptr;
		break;
	case operators::id::bitwise_inverse:
		if (!is_left)
			return nullptr;
		is_integral = true;
		break;
	case operators::id::increment:
	case operators::id::decrement:
		is_integral = true;
		is_lval = true;
		break;
	default:
		return nullptr;
	}

	auto target_type = target->get_type();
	if (target_type == nullptr)
		throw exception::invalid_type();

	auto target_number_type = target_type->as<type::number_primitive>();
	if (target_number_type == nullptr)
		throw exception::unsupported_op();

	if (is_integral && !target_number_type->is_integral())
		throw exception::unsupported_op();

	if (is_lval){
		if (!target->is_lvalue())
			throw exception::rval_assignment();

		if (target->is_const())
			throw exception::const_assignment();
	}

	object::memory_ptr_type result;
	switch (target_number_type->get_state()){
	case type::number_primitive::state_type::small_integer:
		if (result = evaluate_integral_<__int16>(op, target, is_left); result != nullptr)
			return result;
		return evaluate_<__int16>(op, target, is_left);
	case type::number_primitive::state_type::integer:
		if (result = evaluate_integral_<__int32>(op, target, is_left); result != nullptr)
			return result;
		return evaluate_<__int32>(op, target, is_left);
	case type::number_primitive::state_type::big_integer:
		if (result = evaluate_integral_<__int64>(op, target, is_left); result != nullptr)
			return result;
		return evaluate_<__int64>(op, target, is_left);
	case type::number_primitive::state_type::unsigned_small_integer:
		if (result = evaluate_integral_<unsigned __int16>(op, target, is_left); result != nullptr)
			return result;
		return evaluate_unsigned_<unsigned __int16>(op, target, is_left);
	case type::number_primitive::state_type::unsigned_integer:
		if (result = evaluate_integral_<unsigned __int32>(op, target, is_left); result != nullptr)
			return result;
		return evaluate_unsigned_<unsigned __int32>(op, target, is_left);
	case type::number_primitive::state_type::unsigned_big_integer:
		if (result = evaluate_integral_<unsigned __int64>(op, target, is_left); result != nullptr)
			return result;
		return evaluate_unsigned_<unsigned __int64>(op, target, is_left);
	case type::number_primitive::state_type::small_float:
		return evaluate_<float>(op, target, is_left);
	case type::number_primitive::state_type::float_:
		return evaluate_<double>(op, target, is_left);
	case type::number_primitive::state_type::big_float:
		return evaluate_<long double>(op, target, is_left);
	default:
		break;
	}

	throw exception::unsupported_op();
	return nullptr;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::arithmetic::evaluate(operators::id op, object::memory_ptr_type left_value, object::node_ptr_type right) const{
	auto is_integral = false, left_has_precedence = false;
	switch (op){
	case operators::id::plus:
	case operators::id::minus:
	case operators::id::times:
	case operators::id::divide:
		break;
	case operators::id::modulus:
		is_integral = true;
		break;
	case operators::id::bitwise_or:
	case operators::id::bitwise_xor:
	case operators::id::bitwise_and:
	case operators::id::left_shift:
	case operators::id::right_shift:
	case operators::id::index:
		is_integral = true;
		left_has_precedence = true;
		break;
	default:
		return nullptr;
	}

	if (left_value == nullptr)
		throw exception::unsupported_op();

	auto right_value = right->evaluate();
	if (right_value == nullptr)
		throw exception::unsupported_op();

	auto left_type = left_value->get_type(), right_type = right_value->get_type();
	if (left_type == nullptr || right_type == nullptr)
		throw exception::invalid_type();

	auto left_number_type = left_type->as<type::number_primitive>();
	if (left_number_type == nullptr)
		throw exception::unsupported_op();

	auto compatible_left_value = left_value, compatible_right_value = right_value;
	auto right_number_type = right_type->as<type::number_primitive>();

	if (right_number_type == nullptr){
		if (right_type->is<type::string>()){
			if (op != operators::id::plus)
				throw exception::unsupported_op();

			auto left_string_value = left_number_type->get_string_value(left_value);
			auto right_string_value = runtime::object::global_storage->get_string_value(right_value);

			return runtime::object::global_storage->create_string(left_string_value + right_string_value.data());
		}

		if ((compatible_right_value = right_type->cast(right_value, left_type, type::cast_type::static_rval)) != nullptr){
			right_type = left_type;
			right_number_type = left_number_type;
		}
	}
	else if (left_has_precedence || left_number_type->get_precedence(*right_number_type) == left_number_type->get_state()){
		compatible_right_value = right_type->cast(right_value, left_type, type::cast_type::static_rval);
		right_type = left_type;
		right_number_type = left_number_type;
	}
	else{//Convert left
		compatible_left_value = left_type->cast(left_value, right_type, type::cast_type::static_rval);
		left_type = right_type;
		left_number_type = right_number_type;
	}

	if (compatible_left_value == nullptr || compatible_right_value == nullptr)
		throw exception::incompatible_rval();

	if (is_integral && !left_number_type->is_integral())
		throw exception::unsupported_op();

	switch (left_number_type->get_state()){
	case type::number_primitive::state_type::small_integer:
		return evaluate_integral_<__int16>(op, compatible_left_value, compatible_right_value);
	case type::number_primitive::state_type::integer:
		return evaluate_integral_<__int32>(op, compatible_left_value, compatible_right_value);
	case type::number_primitive::state_type::big_integer:
		return evaluate_integral_<__int64>(op, compatible_left_value, compatible_right_value);
	case type::number_primitive::state_type::unsigned_small_integer:
		return evaluate_integral_<unsigned __int16>(op, compatible_left_value, compatible_right_value);
	case type::number_primitive::state_type::unsigned_integer:
		return evaluate_integral_<unsigned __int32>(op, compatible_left_value, compatible_right_value);
	case type::number_primitive::state_type::unsigned_big_integer:
		return evaluate_integral_<unsigned __int64>(op, compatible_left_value, compatible_right_value);
	case type::number_primitive::state_type::small_float:
		return evaluate_<float>(op, compatible_left_value, compatible_right_value);
	case type::number_primitive::state_type::float_:
		return evaluate_<double>(op, compatible_left_value, compatible_right_value);
	case type::number_primitive::state_type::big_float:
		return evaluate_<long double>(op, compatible_left_value, compatible_right_value);
	default:
		break;
	}

	throw exception::unsupported_op();
	return nullptr;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::arithmetic::create_byte_val_(std::byte value) const{
	return std::make_shared<memory::scalar_reference<std::byte>>(runtime::object::global_storage->get_cached_type(storage::global::cached_type::byte_), value);
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::arithmetic::create_byte_ref_(std::size_t address, bool is_const) const{
	return std::make_shared<memory::reference>(
		address,
		runtime::object::global_storage->get_ref_type(runtime::object::global_storage->get_cached_type(storage::global::cached_type::byte_), is_const),
		nullptr
	);
}
