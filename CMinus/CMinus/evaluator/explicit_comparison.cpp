#include "../storage/global_storage.h"

#include "evaluator_object.h"

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

	if (!left_type->remove_const_ref()->is_exact(*right_type->remove_const_ref()))
		return runtime::object::global_storage->get_boolean_value(!is_explicit_equal);

	auto evaluator = dynamic_cast<const object *>(this);
	if (evaluator == nullptr)
		throw exception::unsupported_op();

	return evaluator->evaluate_binary((is_explicit_equal ? operators::id::equal : operators::id::not_equal), left_value, right_value);
}
