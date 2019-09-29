#include "number_evaluator.h"

cminus::evaluator::number::~number() = default;

cminus::evaluator::object::id_type cminus::evaluator::number::get_id() const{
	return id_type::number;
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::number::evaluate_unary_left(operators::id op, memory_ptr_type target) const{
	return arithmetic::evaluate(op, target, true);
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::number::evaluate_unary_right(operators::id op, memory_ptr_type target) const{
	return arithmetic::evaluate(op, target, false);
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::number::evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const{
	if (assignment::assign(op, left_value, right) || compound_assignment::assign(op, left_value, right))
		return left_value;

	if (auto result = numeric_comparison::evaluate(op, left_value, right); result != nullptr)
		return result;

	if (auto result = arithmetic::evaluate(op, left_value, right); result != nullptr)
		return result;

	return nullptr;
}
