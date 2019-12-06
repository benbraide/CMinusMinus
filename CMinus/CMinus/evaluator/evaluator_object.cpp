#include "../node/memory_reference_node.h"

#include "evaluator_object.h"

cminus::evaluator::object::~object() = default;

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::object::evaluate_binary(operators::id op, memory_ptr_type left_value, node_ptr_type right) const{
	if (auto result = explicit_comparison::evaluate(op, left_value, right); result != nullptr)
		return result;//Handled
	return evaluate_binary_(op, left_value, right);
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::object::evaluate_binary(operators::id op, memory_ptr_type left_value, memory_ptr_type right_value) const{
	return evaluate_binary(op, left_value, std::make_shared<node::memory_reference>(right_value));
}

cminus::evaluator::noop::~noop() = default;

cminus::evaluator::object::id_type cminus::evaluator::noop::get_id() const{
	return id_type::nil;
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::noop::evaluate_unary_left(operators::id op, memory_ptr_type target) const{
	return nullptr;
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::noop::evaluate_unary_right(operators::id op, memory_ptr_type target) const{
	return nullptr;
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::noop::evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const{
	return nullptr;
}
