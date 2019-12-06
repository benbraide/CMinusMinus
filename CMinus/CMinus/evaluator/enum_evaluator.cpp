#include "../storage/global_storage.h"

#include "enum_evaluator.h"

cminus::evaluator::enum_::~enum_() = default;

cminus::evaluator::object::id_type cminus::evaluator::enum_::get_id() const{
	return id_type::enum_;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::enum_::evaluate_unary_left(operators::id op, memory_ptr_type target) const{
	return nullptr;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::enum_::evaluate_unary_right(operators::id op, memory_ptr_type target) const{
	return nullptr;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::enum_::evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const{
	if (assignment::assign(op, left_value, right))
		return left_value;

	if (op != operators::id::equal && op != operators::id::not_equal)
		return nullptr;

	auto right_value = right->evaluate();
	if (right_value == nullptr)
		throw exception::unsupported_op();

	auto left_type = left_value->get_type(), right_type = right_value->get_type();
	if (left_type == nullptr || right_type == nullptr)
		throw exception::invalid_type();

	auto compatible_right_value = right_type->cast(right_value, left_type, type::cast_type::static_rval);
	if (compatible_right_value == nullptr)
		throw exception::unsupported_op();

	return runtime::object::global_storage->get_boolean_value(runtime::object::global_storage->compare_enum(*left_type, left_value, right_value) == (op == operators::id::equal));
}
