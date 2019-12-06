#include "../storage/global_storage.h"

#include "boolean_evaluator.h"

cminus::evaluator::boolean::~boolean() = default;

cminus::evaluator::object::id_type cminus::evaluator::boolean::get_id() const{
	return id_type::boolean;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::boolean::evaluate_unary_left(operators::id op, memory_ptr_type target) const{
	if (op == operators::id::relational_not)
		return runtime::object::global_storage->get_boolean_value(target->read_scalar<type::boolean_constant>() == type::boolean_constant::false_);
	return nullptr;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::boolean::evaluate_unary_right(operators::id op, memory_ptr_type target) const{
	return nullptr;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::boolean::evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const{
	if (assignment::assign(op, left_value, right))
		return left_value;

	if (op == operators::id::equal || op == operators::id::not_equal)
		return generic_comparison::evaluate(op, left_value, right);

	if (op != operators::id::relational_or && op != operators::id::relational_and)
		return nullptr;

	auto boolean_value = (left_value->read_scalar<type::boolean_constant>() == type::boolean_constant::true_);
	if (boolean_value && op == operators::id::relational_or)
		return runtime::object::global_storage->get_boolean_value(true);

	if (!boolean_value && op == operators::id::relational_and)
		return runtime::object::global_storage->get_boolean_value(false);

	auto right_value = right->evaluate();
	if (right_value == nullptr)
		throw exception::unsupported_op();

	auto right_type = right_value->get_type();
	if (right_type == nullptr)
		throw exception::invalid_type();

	auto compatible_right_value = right_type->cast(right_value, runtime::object::global_storage->get_boolean_type(), type::cast_type::static_rval);
	if (compatible_right_value == nullptr)
		throw exception::unsupported_op();

	return runtime::object::global_storage->get_boolean_value(compatible_right_value->read_scalar<type::boolean_constant>() == type::boolean_constant::true_);
}
