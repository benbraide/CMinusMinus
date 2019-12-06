#include "../storage/global_storage.h"

#include "type_evaluator.h"

cminus::evaluator::type_value::~type_value() = default;

cminus::evaluator::object::id_type cminus::evaluator::type_value::get_id() const{
	return id_type::type;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::type_value::evaluate_unary_left(operators::id op, memory_ptr_type target) const{
	return nullptr;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::type_value::evaluate_unary_right(operators::id op, memory_ptr_type target) const{
	return nullptr;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::type_value::evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const{
	if (assignment::assign(op, left_value, right))
		return left_value;

	if (op != operators::id::equal && op != operators::id::not_equal && op != operators::id::like)
		return nullptr;

	auto right_value = right->evaluate();
	if (right_value == nullptr)
		throw exception::unsupported_op();

	auto left_type = left_value->get_type(), right_type = right_value->get_type();
	if (left_type == nullptr || right_type == nullptr)
		throw exception::invalid_type();

	auto compatible_value = right_type->cast(right_value, left_type, type::cast_type::static_rval);
	if (compatible_value == nullptr)
		throw exception::incompatible_rval();

	auto left_type_value = left_value->read_scalar<type::object *>(), right_type_value = right_value->read_scalar<type::object *>();
	if (op != operators::id::like)
		return runtime::object::global_storage->get_boolean_value(left_type_value->is_exact(*right_type_value) == (op == operators::id::equal));

	return runtime::object::global_storage->get_boolean_value(left_type_value->get_score(*right_type_value, false, false) != type::object::get_score_value(type::object::score_result_type::nil));
}
