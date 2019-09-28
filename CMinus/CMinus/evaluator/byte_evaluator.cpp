#include "../storage/global_storage.h"

#include "byte_evaluator.h"

cminus::evaluator::byte::~byte() = default;

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::byte::evaluate_unary_left(operators::id op, memory_ptr_type target) const{
	if (op == operators::id::bitwise_inverse && target->get_type()->is(type::object::query_type::byte)){
		attribute::read_guard guard(target, nullptr);
		return std::make_shared<memory::scalar_reference<std::byte>>(target->get_type(), ~target->read_scalar<std::byte>());
	}

	return nullptr;
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::byte::evaluate_unary_right(operators::id op, memory_ptr_type target) const{
	return nullptr;
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::byte::evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const{
	if (assignment::assign(op, left_value, right))
		return left_value;

	if (auto result = generic_comparison::evaluate(op, left_value, right); result != nullptr)
		return result;

	auto left_type = left_value->get_type();
	if (left_type == nullptr)
		throw exception::invalid_type();

	if (op != operators::id::index || !left_type->is(type::object::query_type::byte))
		return nullptr;

	auto right_value = right->evaluate();
	if (right_value == nullptr)
		throw exception::unsupported_op();

	auto right_type = right_value->get_type();
	if (right_type == nullptr)
		throw exception::invalid_type();

	auto right_number_type = dynamic_cast<type::number_primitive *>(right_type->get_non_proxy());
	if (right_number_type == nullptr || !right_type->is(type::object::query_type::integral))
		throw exception::unsupported_op();

	attribute::read_guard left_read_guard(left_value, nullptr);
	attribute::read_guard right_read_guard(right_value, nullptr);

	auto index = right_number_type->read_value<std::size_t>(right_value);
	if (8u <= index)
		throw runtime::exception::out_of_range();

	return runtime::object::global_storage->get_boolean_value(std::bitset<8u>(static_cast<std::size_t>(left_value->read_scalar<std::byte>())).test(index));
}
