#include "../storage/global_storage.h"

#include "boolean_evaluator.h"

cminus::evaluator::boolean::~boolean() = default;

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::boolean::evaluate_unary_left(operators::id op, memory_ptr_type target) const{
	if (op == operators::id::relational_not){
		attribute::read_guard guard(target, nullptr);
		return runtime::object::global_storage->get_boolean_value(target->read_scalar<type::boolean_constant>() == type::boolean_constant::false_);
	}

	return nullptr;
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::boolean::evaluate_unary_right(operators::id op, memory_ptr_type target) const{
	return nullptr;
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::boolean::evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const{
	if (assignment::assign(op, left_value, right))
		return left_value;

	if (op != operators::id::equal && op == operators::id::not_equal)
		return nullptr;

	return generic_comparison::evaluate(op, left_value, right);
}
