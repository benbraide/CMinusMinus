#include "../storage/global_storage.h"
#include "../evaluator/evaluator_exception.h"

#include "expression_node.h"

cminus::node::left_unary_expression::left_unary_expression(operators::id op, std::shared_ptr<object> operand)
	: op_(op), operand_(operand){}

cminus::node::left_unary_expression::~left_unary_expression() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::left_unary_expression::evaluate() const{
	auto value = operand_->evaluate();
	if (value == nullptr)
		throw evaluator::exception::unsupported_op();

	return value->get_type()->get_evaluator()->evaluate_unary_left(op_, value);
}

cminus::node::right_unary_expression::right_unary_expression(operators::id op, std::shared_ptr<object> operand)
	: op_(op), operand_(operand){}

cminus::node::right_unary_expression::~right_unary_expression() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::right_unary_expression::evaluate() const{
	auto value = operand_->evaluate();
	if (value == nullptr)
		throw evaluator::exception::unsupported_op();

	return value->get_type()->get_evaluator()->evaluate_unary_right(op_, value);
}

cminus::node::binary_expression::binary_expression(operators::id op, std::shared_ptr<object> left_operand, std::shared_ptr<object> right_operand)
	: op_(op), left_operand_(left_operand), right_operand_(right_operand){}

cminus::node::binary_expression::~binary_expression() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::binary_expression::evaluate() const{
	auto left_value = left_operand_->evaluate();
	if (left_value == nullptr)
		throw evaluator::exception::unsupported_op();

	return left_value->get_type()->get_evaluator()->evaluate_binary(op_, left_value, right_operand_);
}
