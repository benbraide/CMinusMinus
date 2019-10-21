#include "compound_assignment.h"

cminus::evaluator::compound_assignment::~compound_assignment() = default;

bool cminus::evaluator::compound_assignment::assign(operators::id op, object::memory_ptr_type left_value, object::node_ptr_type right) const{
	auto is_integral = false;
	switch (op){
	case operators::id::compound_plus:
	case operators::id::compound_minus:
	case operators::id::compound_times:
	case operators::id::compound_divide:
		break;
	case operators::id::compound_modulus:
	case operators::id::compound_bitwise_or:
	case operators::id::compound_bitwise_xor:
	case operators::id::compound_bitwise_and:
	case operators::id::compound_left_shift:
	case operators::id::compound_right_shift:
		is_integral = true;
		break;
	default:
		return false;
	}

	if (left_value == nullptr)
		throw exception::unsupported_op();

	if (!left_value->is_lvalue())
		throw exception::rval_assignment();

	if (left_value->is_const())
		throw exception::const_assignment();

	auto right_value = right->evaluate();
	if (right_value == nullptr)
		throw exception::unsupported_op();

	auto left_type = left_value->get_type(), right_type = right_value->get_type();
	if (left_type == nullptr || right_type == nullptr)
		throw exception::invalid_type();

	auto left_number_type = left_type->as<type::number_primitive>();
	if (left_number_type == nullptr)
		throw exception::unsupported_op();

	if (is_integral && !left_number_type->is_integral())
		throw exception::unsupported_op();

	auto compatible_value = right_type->cast(right_value, right_type, type::cast_type::static_rval);
	if (compatible_value == nullptr)
		throw exception::incompatible_rval();

	switch (left_number_type->get_state()){
	case type::number_primitive::state_type::small_integer:
		return evaluate_and_assign_integral_<__int16>(op, left_value, right_value);
	case type::number_primitive::state_type::integer:
		return evaluate_and_assign_integral_<__int32>(op, left_value, right_value);
	case type::number_primitive::state_type::big_integer:
		return evaluate_and_assign_integral_<__int64>(op, left_value, right_value);
	case type::number_primitive::state_type::unsigned_small_integer:
		return evaluate_and_assign_integral_<unsigned __int16>(op, left_value, right_value);
	case type::number_primitive::state_type::unsigned_integer:
		return evaluate_and_assign_integral_<unsigned __int32>(op, left_value, right_value);
	case type::number_primitive::state_type::unsigned_big_integer:
		return evaluate_and_assign_integral_<unsigned __int64>(op, left_value, right_value);
	case type::number_primitive::state_type::small_float:
		return evaluate_and_assign_<float>(op, left_value, right_value);
	case type::number_primitive::state_type::float_:
		return evaluate_and_assign_<double>(op, left_value, right_value);
	case type::number_primitive::state_type::big_float:
		return evaluate_and_assign_<long double>(op, left_value, right_value);
	default:
		break;
	}

	throw exception::unsupported_op();
	return false;
}
