#pragma once

#include "../operators/operator_id.h"

#include "node_object.h"

namespace cminus::node{
	class left_unary_expression : public object{
	public:
		left_unary_expression(operators::id op, std::shared_ptr<object> operand);

		virtual ~left_unary_expression();

		virtual std::shared_ptr<memory::reference> evaluate() const override;

	protected:
		operators::id op_;
		std::shared_ptr<object> operand_;
	};

	class right_unary_expression : public object{
	public:
		right_unary_expression(operators::id op, std::shared_ptr<object> operand);

		virtual ~right_unary_expression();

		virtual std::shared_ptr<memory::reference> evaluate() const override;

	protected:
		operators::id op_;
		std::shared_ptr<object> operand_;
	};

	class binary_expression : public object{
	public:
		binary_expression(operators::id op, std::shared_ptr<object> left_operand, std::shared_ptr<object> right_operand);

		virtual ~binary_expression();

		virtual std::shared_ptr<memory::reference> evaluate() const override;

	protected:
		operators::id op_;
		std::shared_ptr<object> left_operand_;
		std::shared_ptr<object> right_operand_;
	};
}
