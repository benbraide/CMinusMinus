#include "pointer_evaluator.h"

cminus::evaluator::pointer::~pointer() = default;

cminus::evaluator::object::id_type cminus::evaluator::pointer::get_id() const{
	return id_type::pointer;
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::pointer::evaluate_unary_left(operators::id op, memory_ptr_type target) const{
	auto is_lval = (op == operators::id::increment || op == operators::id::decrement);
	if (!is_lval && op != operators::id::times)
		return nullptr;

	auto target_type = target->get_type();
	if (target_type == nullptr)
		throw exception::invalid_type();

	auto pointer_target_type = target_type->as<type::pointer_primitive>();
	if (pointer_target_type == nullptr)
		throw exception::unsupported_op();

	auto base_type = pointer_target_type->get_base_type();
	if (base_type == nullptr)
		throw exception::unsupported_op();

	auto base_size = base_type->get_size();
	if (base_size == 0u)
		throw exception::unsupported_op();

	if (is_lval){//Increment | Decrement
		if (!target->is_lvalue())
			throw exception::rval_assignment();

		if (target->is_const())
			throw exception::const_assignment();

		if (pointer_target_type->is_reversed()){
			if (op == operators::id::increment)
				target->write(std::make_shared<memory::scalar_reference<std::size_t>>(target_type, (target->read_scalar<std::size_t>() - base_size)));
			else//Decrement
				target->write(std::make_shared<memory::scalar_reference<std::size_t>>(target_type, (target->read_scalar<std::size_t>() + base_size)));
		}
		else if (op == operators::id::increment)
			target->write(std::make_shared<memory::scalar_reference<std::size_t>>(target_type, (target->read_scalar<std::size_t>() + base_size)));
		else
			target->write(std::make_shared<memory::scalar_reference<std::size_t>>(target_type, (target->read_scalar<std::size_t>() - base_size)));

		return target;
	}

	return std::make_shared<memory::reference>(target->read_scalar<std::size_t>(), base_type);
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::pointer::evaluate_unary_right(operators::id op, memory_ptr_type target) const{
	if (op != operators::id::increment && op != operators::id::decrement)
		return nullptr;

	if (!target->is_lvalue())
		throw exception::rval_assignment();

	if (target->is_const())
		throw exception::const_assignment();

	auto target_type = target->get_type();
	if (target_type == nullptr)
		throw exception::invalid_type();

	auto pointer_target_type = target_type->as<type::pointer_primitive>();
	if (pointer_target_type == nullptr)
		throw exception::unsupported_op();

	auto base_type = pointer_target_type->get_base_type();
	if (base_type == nullptr)
		throw exception::unsupported_op();

	auto base_size = base_type->get_size();
	if (base_size == 0u)
		throw exception::unsupported_op();

	auto old_value = target->read_scalar<std::size_t>();
	if (pointer_target_type->is_reversed()){
		if (op == operators::id::increment)
			target->write(std::make_shared<memory::scalar_reference<std::size_t>>(target_type, (old_value - base_size)));
		else//Decrement
			target->write(std::make_shared<memory::scalar_reference<std::size_t>>(target_type, (old_value + base_size)));
	}
	else if (op == operators::id::increment)
		target->write(std::make_shared<memory::scalar_reference<std::size_t>>(target_type, (old_value + base_size)));
	else//Decrement
		target->write(std::make_shared<memory::scalar_reference<std::size_t>>(target_type, (old_value - base_size)));

	return std::make_shared<memory::scalar_reference<std::size_t>>(target_type, old_value);
}

cminus::evaluator::object::memory_ptr_type cminus::evaluator::pointer::evaluate_binary_(operators::id op, memory_ptr_type left_value, node_ptr_type right) const{
	if (assignment::assign(op, left_value, right))
		return left_value;

	if (auto result = pointer_comparison::evaluate(op, left_value, right); result != nullptr)
		return result;

	auto is_lval = false;
	switch (op){
	case operators::id::index:
	case operators::id::plus:
	case operators::id::minus:
		break;
	case operators::id::compound_plus:
	case operators::id::compound_minus:
		is_lval = true;
		break;
	default:
		return nullptr;
	}

	auto right_value = right->evaluate();
	if (right_value == nullptr)
		throw exception::unsupported_op();

	auto left_type = left_value->get_type(), right_type = right_value->get_type();
	if (left_type == nullptr || right_type == nullptr)
		throw exception::invalid_type();

	auto left_pointer_type = left_type->as<type::pointer_primitive>();
	if (left_pointer_type == nullptr)
		throw exception::unsupported_op();

	auto left_base_type = left_pointer_type->get_base_type();
	if (left_base_type == nullptr)
		throw exception::unsupported_op();

	auto left_base_size = left_base_type->get_size();
	if (left_base_size == 0u)
		throw exception::unsupported_op();

	auto right_number_type = right_type->as<type::number_primitive>();
	if (right_number_type == nullptr || !right_number_type->is_integral())
		throw exception::unsupported_op();

	auto value = (right_number_type->read_value<std::size_t>(right_value) * left_base_size);
	if (is_lval){
		if (!left_value->is_lvalue())
			throw exception::rval_assignment();

		if (left_value->is_const())
			throw exception::const_assignment();

		if (left_pointer_type->is_reversed()){
			if (op == operators::id::compound_plus)
				left_value->write(std::make_shared<memory::scalar_reference<std::size_t>>(left_type, (left_value->read_scalar<std::size_t>() - value)));
			else
				left_value->write(std::make_shared<memory::scalar_reference<std::size_t>>(left_type, (left_value->read_scalar<std::size_t>() + value)));
		}
		else if (op == operators::id::compound_plus)
			left_value->write(std::make_shared<memory::scalar_reference<std::size_t>>(left_type, (left_value->read_scalar<std::size_t>() + value)));
		else
			left_value->write(std::make_shared<memory::scalar_reference<std::size_t>>(left_type, (left_value->read_scalar<std::size_t>() - value)));

		return left_value;
	}

	if (op == operators::id::plus){
		if (left_pointer_type->is_reversed())
			return std::make_shared<memory::scalar_reference<std::size_t>>(left_type, (left_value->read_scalar<std::size_t>() - value));
		return std::make_shared<memory::scalar_reference<std::size_t>>(left_type, (left_value->read_scalar<std::size_t>() + value));
	}

	if (op == operators::id::minus){
		if (left_pointer_type->is_reversed())
			return std::make_shared<memory::scalar_reference<std::size_t>>(left_type, (left_value->read_scalar<std::size_t>() + value));
		return std::make_shared<memory::scalar_reference<std::size_t>>(left_type, (left_value->read_scalar<std::size_t>() - value));
	}

	if (left_pointer_type->is_reversed()){
		return std::make_shared<memory::reference>(
			(left_value->read_scalar<std::size_t>() - value),
			left_base_type
		);
	}

	return std::make_shared<memory::reference>(
		(left_value->read_scalar<std::size_t>() + value),
		left_base_type
	);
}
