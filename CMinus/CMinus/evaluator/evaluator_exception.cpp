#include "evaluator_exception.h"

cminus::evaluator::exception::unnamed::unnamed(code code)
	: base("Unknown evaluator error"), code_(code){}

cminus::evaluator::exception::unnamed::~unnamed() = default;

cminus::evaluator::exception::code cminus::evaluator::exception::unnamed::get_code() const{
	return code_;
}

cminus::evaluator::exception::invalid_type::invalid_type()
	: base("An invalid was encountered in an expression"){}

cminus::evaluator::exception::invalid_type::~invalid_type() = default;

cminus::evaluator::exception::code cminus::evaluator::exception::invalid_type::get_code() const{
	return code::invalid_type;
}

cminus::evaluator::exception::incompatible_rval::incompatible_rval()
	: base("Cannot assign specified value to target"){}

cminus::evaluator::exception::incompatible_rval::~incompatible_rval() = default;

cminus::evaluator::exception::code cminus::evaluator::exception::incompatible_rval::get_code() const{
	return code::incompatible_rval;
}

cminus::evaluator::exception::rval_assignment::rval_assignment()
	: base("Assignment requires an l-value target"){}

cminus::evaluator::exception::rval_assignment::~rval_assignment() = default;

cminus::evaluator::exception::code cminus::evaluator::exception::rval_assignment::get_code() const{
	return code::rval_assignment;
}

cminus::evaluator::exception::const_assignment::const_assignment()
	: base("Cannot assignment value to a constant target"){}

cminus::evaluator::exception::const_assignment::~const_assignment() = default;

cminus::evaluator::exception::code cminus::evaluator::exception::const_assignment::get_code() const{
	return code::const_assignment;
}

cminus::evaluator::exception::unsupported_op::unsupported_op()
	: base("The operator does not take specified operand(s)"){}

cminus::evaluator::exception::unsupported_op::~unsupported_op() = default;

cminus::evaluator::exception::code cminus::evaluator::exception::unsupported_op::get_code() const{
	return code::unsupported_op;
}
