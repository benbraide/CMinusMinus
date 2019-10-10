#include "../storage/global_storage.h"

#include "enum_evaluator.h"

cminus::evaluator::enum_::~enum_() = default;

cminus::evaluator::object::id_type cminus::evaluator::enum_::get_id() const{
	return id_type::enum_;
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::enum_::evaluate_unary_left(operators::id op, memory_ptr_type target) const{
	return nullptr;
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::enum_::evaluate_unary_right(operators::id op, memory_ptr_type target) const{
	return nullptr;
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::enum_::evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const{
	if (assignment::assign(op, left_value, right))
		return left_value;

	if (op != operators::id::equal && op != operators::id::not_equal && op != operators::id::spaceship)
		return nullptr;

	auto right_value = right->evaluate();
	if (right_value == nullptr)
		throw exception::unsupported_op();

	auto left_type = left_value->get_type(), right_type = right_value->get_type();
	if (left_type == nullptr || right_type == nullptr)
		throw exception::invalid_type();

	attribute::read_guard left_read_guard(left_value, nullptr);
	attribute::read_guard right_read_guard(right_value, nullptr);

	auto compatible_right_value = right_type->cast(right_value, left_type, type::cast_type::rval_static);
	if (compatible_right_value == nullptr)
		throw exception::unsupported_op();

	auto result = runtime::object::global_storage->compare_enum(*left_type, left_value, right_value);
	if (op == operators::id::spaceship){
		if (result)
			runtime::object::global_storage->get_compare_value(0);
		return runtime::object::global_storage->get_not_equal_compare_value();
	}

	return runtime::object::global_storage->get_boolean_value(result  == (op == operators::id::equal));
}
