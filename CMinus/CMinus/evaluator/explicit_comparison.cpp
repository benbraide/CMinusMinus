#include "../storage/global_storage.h"

#include "comparison.h"

cminus::evaluator::explicit_comparison::~explicit_comparison() = default;

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::explicit_comparison::evaluate(operators::id op, memory_ptr_type left_value, node_ptr_type right) const{
	auto is_explicit_equal = (op == operators::id::explicit_equal);
	if (!is_explicit_equal && op != operators::id::explicit_not_equal)
		return nullptr;

	if (left_value == nullptr)
		throw exception::unsupported_op();

	memory_ptr_type right_value;
	try{
		right_value = right->evaluate();
	}
	catch (const storage::exception::entry_not_found &){
		right_value = runtime::object::global_storage->get_undefined_value();
	}

	if (right_value == nullptr)
		throw exception::unsupported_op();

	auto left_type = left_value->get_type(), right_type = right_value->get_type();
	if (left_type == nullptr || right_type == nullptr)
		throw exception::invalid_type();

	auto non_ref_const_left_type = left_type->convert(type::object::conversion_type::remove_ref_const, left_type);
	auto non_ref_const_right_type = right_type->convert(type::object::conversion_type::remove_ref_const, right_type);

	if (!non_ref_const_left_type->is_exact(*non_ref_const_right_type))
		return runtime::object::global_storage->get_boolean_value(!is_explicit_equal);

	auto comparison_evaluator = dynamic_cast<const comparison *>(this);
	if (comparison_evaluator == nullptr)
		throw exception::unsupported_op();

	return comparison_evaluator->evaluate((is_explicit_equal ? operators::id::equal : operators::id::not_equal), left_value, right_value);
}
