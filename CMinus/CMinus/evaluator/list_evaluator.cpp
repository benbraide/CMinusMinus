#include "../storage/global_storage.h"

#include "list_evaluator.h"

cminus::evaluator::list::~list() = default;

cminus::evaluator::object::id_type cminus::evaluator::list::get_id() const{
	return id_type::array_;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::list::evaluate_unary_left(operators::id op, memory_ptr_type target) const{
	return nullptr;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::list::evaluate_unary_right(operators::id op, memory_ptr_type target) const{
	return nullptr;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::list::evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const{
	if (assignment::assign(op, left_value, right))
		return left_value;

	if (op != operators::id::index)
		return nullptr;

	auto right_value = right->evaluate();
	if (right_value == nullptr)
		throw exception::unsupported_op();

	auto left_type = left_value->get_type(), right_type = right_value->get_type();
	if (left_type == nullptr || right_type == nullptr)
		throw exception::invalid_type();

	auto left_array_type = left_type->as<type::array_primitive>();
	if (left_array_type == nullptr)
		throw exception::unsupported_op();

	auto left_base_type = left_array_type->get_base_type();
	if (left_base_type == nullptr)
		throw exception::unsupported_op();

	auto left_base_size = left_base_type->get_memory_size();
	if (left_base_size == 0u)
		throw exception::unsupported_op();

	auto right_number_type = right_type->as<type::number_primitive>();
	if (right_number_type == nullptr || !right_number_type->is_integral())
		throw exception::unsupported_op();

	auto value = right_number_type->read_value<std::size_t>(right_value);
	if (left_array_type->get_count() <= value)
		throw runtime::exception::out_of_range();

	if (left_base_type->is_ref()){
		return std::make_shared<memory::indirect_reference>(
			(left_value->get_address() + (value * left_base_size)),
			left_base_type->remove_const_ref(left_base_type)
		);
	}

	return std::make_shared<memory::reference>(
		(left_value->get_address() + (value * left_base_size)),
		left_base_type->remove_const_ref(left_base_type)
	);
}
