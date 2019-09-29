#include "pointer_evaluator.h"

cminus::evaluator::pointer::~pointer() = default;

cminus::evaluator::object::id_type cminus::evaluator::pointer::get_id() const{
	return id_type::pointer;
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::pointer::evaluate_unary_left(operators::id op, memory_ptr_type target) const{
	auto is_dereference = (op == operators::id::times);
	if (!is_dereference && op != operators::id::increment && op != operators::id::decrement)
		return nullptr;

	if (!is_dereference){//Increment | Decrement
		if (!target->is_lvalue())
			throw exception::rval_assignment();

		if (target->is_const())
			throw exception::const_assignment();
	}

	auto target_type = target->get_type();
	if (target_type == nullptr)
		throw exception::invalid_type();

	auto base_type = target_type->convert(type::object::conversion_type::remove_pointer, target_type);
	if (base_type == nullptr)
		throw exception::unsupported_op();

	if (!is_dereference){//Increment | Decrement
		attribute::write_read_guard guard(target, nullptr);
		return evaluate_integral_<__int64>(op, target, true, base_type->get_size());
	}

	return std::make_shared<memory::reference>(target->read_scalar<std::size_t>(), base_type, attribute::collection::list_type{}, nullptr);
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::pointer::evaluate_unary_right(operators::id op, memory_ptr_type target) const{
	if (op != operators::id::increment && op != operators::id::decrement)
		return nullptr;

	if (!target->is_lvalue())
		throw exception::rval_assignment();

	if (target->is_const())
		throw exception::const_assignment();

	auto target_type = target->get_type();
	if (target_type == nullptr)
		throw exception::invalid_type();

	auto base_type = target_type->convert(type::object::conversion_type::remove_pointer, target_type);
	if (base_type == nullptr)
		throw exception::unsupported_op();

	attribute::write_read_guard guard(target, nullptr);
	return evaluate_integral_<__int64>(op, target, false, base_type->get_size());
}

cminus::evaluator::explicit_comparison::memory_ptr_type cminus::evaluator::pointer::evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const{
	if (assignment::assign(op, left_value, right) || compound_assignment::assign(op, left_value, right))
		return left_value;

	if (auto result = pointer_comparison::evaluate(op, left_value, right); result != nullptr)
		return result;

	if (op != operators::id::index)
		return nullptr;

	auto right_value = right->evaluate();
	if (right_value == nullptr)
		throw exception::unsupported_op();

	auto left_type = left_value->get_type(), right_type = right_value->get_type();
	if (left_type == nullptr || right_type == nullptr)
		throw exception::invalid_type();

	auto left_base_type = left_type->convert(type::object::conversion_type::remove_pointer, left_type);
	if (left_base_type == nullptr)
		throw exception::unsupported_op();

	auto right_number_type = dynamic_cast<type::number_primitive *>(right_type->get_non_proxy());
	if (right_number_type == nullptr || !right_type->is(type::object::query_type::integral))
		throw exception::unsupported_op();

	attribute::read_guard left_read_guard(left_value, nullptr);
	attribute::read_guard right_read_guard(right_value, nullptr);

	return std::make_shared<memory::reference>(
		(left_value->read_scalar<std::size_t>() + (left_base_type->get_size() * right_number_type->read_value<std::size_t>(right_value))),
		left_base_type,
		attribute::collection::list_type{},
		nullptr
	);
}
