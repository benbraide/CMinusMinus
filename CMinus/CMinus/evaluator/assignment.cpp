#include "assignment.h"

cminus::evaluator::assignment::~assignment() = default;

bool cminus::evaluator::assignment::assign(operators::id op, object::memory_ptr_type left_value, object::node_ptr_type right) const{
	if (op != operators::id::assignment)
		return false;

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

	auto compatible_value = right_type->cast(right_value, left_type, type::cast_type::static_rval);
	if (compatible_value == nullptr)
		throw exception::incompatible_rval();

	left_value->write(compatible_value);
	return true;
}
