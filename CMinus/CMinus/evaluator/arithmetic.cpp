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

	auto target_number_type = dynamic_cast<type::number_primitive *>(target_type->get_non_proxy());
	if (target_number_type == nullptr)
		throw exception::unsupported_op();

	if (is_integral && !target_type->is(type::object::query_type::integral))
		throw exception::unsupported_op();

	if (is_lval){
		if (!target->is_lvalue())
			throw exception::rval_assignment();

		if (target->is_const())
			throw exception::const_assignment();
	}

	std::shared_ptr<attribute::guard> guard;
	if (is_lval)//Write and read
		guard = std::make_shared<attribute::write_read_guard>(target, nullptr);
	else//Read
		guard = std::make_shared<attribute::read_guard>(target, nullptr);

	object::memory_ptr_type result;
	switch (target_number_type->get_state()){
	case type::number_primitive::state_type::integer:
		if (result = evaluate_integral_<__int32>(op, target, is_left); result != nullptr)
			return result;
		return evaluate_<__int32>(op, target, is_left);
	case type::number_primitive::state_type::long_integer:
		if (result = evaluate_integral_<__int64>(op, target, is_left); result != nullptr)
			return result;
		return evaluate_<__int64>(op, target, is_left);
	case type::number_primitive::state_type::unsigned_integer:
		if (result = evaluate_integral_<unsigned __int32>(op, target, is_left); result != nullptr)
			return result;
		return evaluate_unsigned_<unsigned __int32>(op, target, is_left);
	case type::number_primitive::state_type::unsigned_long_integer:
		if (result = evaluate_integral_<unsigned __int64>(op, target, is_left); result != nullptr)
			return result;
		return evaluate_unsigned_<unsigned __int64>(op, target, is_left);
	case type::number_primitive::state_type::real:
		return evaluate_<float>(op, target, is_left);
	case type::number_primitive::state_type::long_real:
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

	auto left_number_type = dynamic_cast<type::number_primitive *>(left_type->get_non_proxy());
	if (left_number_type == nullptr)
		throw exception::unsupported_op();

	attribute::read_guard left_read_guard(left_value, nullptr);
	attribute::read_guard right_read_guard(right_value, nullptr);

	auto compatible_left_value = left_value, compatible_right_value = right_value;
	auto right_number_type = dynamic_cast<type::number_primitive *>(right_type->get_non_proxy());

	if (right_number_type == nullptr){
		if (right_type->is(type::object::query_type::string)){
			if (op != operators::id::plus)
				throw exception::unsupported_op();

			auto left_string_value = left_number_type->get_string_value(left_value);
			auto right_string_value = runtime::object::global_storage->get_string_value(right_value);

			return runtime::object::global_storage->create_string(left_string_value + right_string_value.data());
		}

		if ((compatible_right_value = right_type->cast(right_value, left_type, type::cast_type::rval_static)) != nullptr){
			right_type = left_type;
			right_number_type = left_number_type;
		}
	}
	else if (left_has_precedence || left_number_type->has_precedence_over(*right_number_type)){
		compatible_right_value = right_type->cast(right_value, left_type, type::cast_type::rval_static);
		right_type = left_type;
		right_number_type = left_number_type;
	}
	else{//Convert left
		compatible_left_value = left_type->cast(left_value, right_type, type::cast_type::rval_static);
		left_type = right_type;
		left_number_type = right_number_type;
	}

	if (compatible_left_value == nullptr || compatible_right_value == nullptr)
		throw exception::incompatible_rval();

	if (is_integral && !left_type->is(type::object::query_type::integral))
		throw exception::unsupported_op();

	switch (left_number_type->get_state()){
	case type::number_primitive::state_type::integer:
		return evaluate_integral_<__int32>(op, compatible_left_value, compatible_right_value);
	case type::number_primitive::state_type::long_integer:
		return evaluate_integral_<__int64>(op, compatible_left_value, compatible_right_value);
	case type::number_primitive::state_type::unsigned_integer:
		return evaluate_integral_<unsigned __int32>(op, compatible_left_value, compatible_right_value);
	case type::number_primitive::state_type::unsigned_long_integer:
		return evaluate_integral_<unsigned __int64>(op, compatible_left_value, compatible_right_value);
	case type::number_primitive::state_type::real:
		return evaluate_<float>(op, compatible_left_value, compatible_right_value);
	case type::number_primitive::state_type::long_real:
		return evaluate_<long double>(op, compatible_left_value, compatible_right_value);
	default:
		break;
	}

	throw exception::unsupported_op();
	return nullptr;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::arithmetic::create_byte_val_(std::byte value) const{
	return nullptr;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::arithmetic::create_byte_ref_(std::size_t address, bool is_const) const{
	return nullptr;
}
