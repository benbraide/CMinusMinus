#include "../storage/global_storage.h"

#include "comparison.h"

cminus::evaluator::comparison::~comparison() = default;

cminus::evaluator::numeric_comparison::~numeric_comparison() = default;

cminus::evaluator::object::memory_ptr_type cminus::evaluator::numeric_comparison::evaluate(operators::id op, object::memory_ptr_type left_value, object::memory_ptr_type right_value) const{
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
			auto left_string_value = left_number_type->get_string_value(left_value);
			auto right_string_value = runtime::object::global_storage->get_string_value(right_value);

			if (op == operators::id::spaceship)
				return create_compare_value_(compare_string_(left_string_value, right_string_value));

			return create_value_(evaluate_string_(op, left_string_value, right_string_value));
		}

		if ((compatible_right_value = right_type->cast(right_value, left_type, type::cast_type::rval_static)) != nullptr){
			right_type = left_type;
			right_number_type = left_number_type;
		}
	}
	else if (left_number_type->has_precedence_over(*right_number_type)){
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

	switch (left_number_type->get_state()){
	case type::number_primitive::state_type::integer:
		if (op == operators::id::spaceship)
			return create_compare_value_(compare_numeric_<__int32>(compatible_left_value, compatible_right_value));
		return create_value_(evaluate_numeric_<__int32>(op, compatible_left_value, compatible_right_value));
	case type::number_primitive::state_type::long_integer:
		if (op == operators::id::spaceship)
			return create_compare_value_(compare_numeric_<__int64>(compatible_left_value, compatible_right_value));
		return create_value_(evaluate_numeric_<__int64>(op, compatible_left_value, compatible_right_value));
	case type::number_primitive::state_type::unsigned_integer:
		if (op == operators::id::spaceship)
			return create_compare_value_(compare_numeric_<unsigned __int32>(compatible_left_value, compatible_right_value));
		return create_value_(evaluate_numeric_<unsigned __int32>(op, compatible_left_value, compatible_right_value));
	case type::number_primitive::state_type::unsigned_long_integer:
		if (op == operators::id::spaceship)
			return create_compare_value_(compare_numeric_<unsigned __int64>(compatible_left_value, compatible_right_value));
		return create_value_(evaluate_numeric_<unsigned __int64>(op, compatible_left_value, compatible_right_value));
	case type::number_primitive::state_type::real:
		if (op == operators::id::spaceship)
			return create_compare_value_(compare_numeric_<float>(compatible_left_value, compatible_right_value));
		return create_value_(evaluate_numeric_<float>(op, compatible_left_value, compatible_right_value));
	case type::number_primitive::state_type::long_real:
		if (op == operators::id::spaceship)
			return create_compare_value_(compare_numeric_<long double>(compatible_left_value, compatible_right_value));
		return create_value_(evaluate_numeric_<long double>(op, compatible_left_value, compatible_right_value));
	default:
		break;
	}

	throw exception::unsupported_op();
	return nullptr;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::numeric_comparison::evaluate(operators::id op, object::memory_ptr_type left_value, object::node_ptr_type right) const{
	auto right_value = pre_evaluate_(op, left_value, right);
	return ((right_value == nullptr) ? nullptr : evaluate(op, left_value, right_value));
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::comparison::pre_evaluate_(operators::id op, object::memory_ptr_type left_value, object::node_ptr_type right) const{
	switch (op){
	case operators::id::less:
	case operators::id::less_or_equal:
	case operators::id::equal:
	case operators::id::not_equal:
	case operators::id::greater_or_equal:
	case operators::id::greater:
	case operators::id::spaceship:
		break;
	default:
		return nullptr;
	}

	if (left_value == nullptr)
		throw exception::unsupported_op();

	auto right_value = right->evaluate();
	if (right_value == nullptr)
		throw exception::unsupported_op();

	return right_value;
}

bool cminus::evaluator::comparison::evaluate_string_(operators::id op, const std::string_view &left, const std::string_view &right) const{
	switch (op){
	case operators::id::less:
		return (left < right);
	case operators::id::less_or_equal:
		return (left <= right);
	case operators::id::equal:
		return (left == right);
	case operators::id::not_equal:
		return (left != right);
	case operators::id::greater_or_equal:
		return (left >= right);
	case operators::id::greater:
		return (left > right);
	default:
		break;
	}

	return false;
}

int cminus::evaluator::comparison::compare_string_(const std::string_view &left, const std::string_view &right) const{
	return left.compare(right);
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::comparison::create_value_(bool value) const{
	return runtime::object::global_storage->get_boolean_value(value);
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::comparison::create_compare_value_(int value) const{
	return runtime::object::global_storage->get_compare_value(value);
}

cminus::evaluator::pointer_comparison::~pointer_comparison() = default;

cminus::evaluator::object::memory_ptr_type cminus::evaluator::pointer_comparison::evaluate(operators::id op, object::memory_ptr_type left_value, object::memory_ptr_type right_value) const{
	auto left_type = left_value->get_type(), right_type = right_value->get_type();
	if (left_type == nullptr || right_type == nullptr)
		throw exception::invalid_type();

	attribute::read_guard left_read_guard(left_value, nullptr);
	attribute::read_guard right_read_guard(right_value, nullptr);

	auto compatible_left_value = left_value, compatible_right_value = right_value;
	if ((compatible_right_value = right_type->cast(right_value, left_type, type::cast_type::rval_static)) == nullptr){//Try casting left value
		if ((compatible_left_value = left_type->cast(left_value, right_type, type::cast_type::rval_static)) != nullptr)
			compatible_right_value = right_value;
		else//Failed both conversions
			throw exception::unsupported_op();
	}

	return create_value_(evaluate_<std::size_t>(op, compatible_left_value, compatible_right_value));
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::pointer_comparison::evaluate(operators::id op, object::memory_ptr_type left_value, object::node_ptr_type right) const{
	auto right_value = pre_evaluate_(op, left_value, right);
	return ((right_value == nullptr) ? nullptr : evaluate(op, left_value, right_value));
}
