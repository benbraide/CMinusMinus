#include "../type/class_type.h"
#include "../storage/global_storage.h"

#include "class_evaluator.h"

cminus::evaluator::class_::~class_() = default;

cminus::evaluator::object::id_type cminus::evaluator::class_::get_id() const{
	return id_type::class_;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::class_::evaluate_unary_left(operators::id op, memory_ptr_type target) const{
	auto callable = find_operator_(op, target->get_type());
	if (callable == nullptr)
		throw exception::unsupported_op();

	memory_ptr_type result;
	try{
		result = callable->call(target, std::vector<memory_ptr_type>{});
	}
	catch (const declaration::exception::function_not_found &){
		throw exception::unsupported_op();
	}

	return result;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::class_::evaluate_unary_right(operators::id op, memory_ptr_type target) const{
	auto callable = find_operator_(op, target->get_type());
	if (callable == nullptr)
		throw exception::unsupported_op();

	memory_ptr_type result;
	try{
		result = callable->call(target, std::vector<memory_ptr_type>{ runtime::object::global_storage->get_zero_value(storage::global::cached_type::integer) });
	}
	catch (const declaration::exception::function_not_found &){
		throw exception::unsupported_op();
	}

	return result;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::class_::evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const{
	auto callable = find_operator_(op, left_value->get_type());
	if (callable == nullptr)
		throw exception::unsupported_op();

	auto right_value = right->evaluate();
	if (right_value == nullptr)
		throw exception::unsupported_op();

	memory_ptr_type result;
	try{
		result = callable->call(left_value, std::vector<memory_ptr_type>{ right_value });
	}
	catch (const declaration::exception::function_not_found &){
		throw exception::unsupported_op();
	}

	return result;
}

cminus::declaration::callable_group *cminus::evaluator::class_::find_operator_(operators::id op, std::shared_ptr<type::object> target_type) const{
	auto class_target_type = target_type->as<type::class_>();
	return ((class_target_type == nullptr) ? nullptr : class_target_type->find_operator(op, false));
}
