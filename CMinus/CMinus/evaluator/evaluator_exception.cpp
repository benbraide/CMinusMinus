#include "../type/system_enum_types.h"
#include "../storage/global_storage.h"

#include "evaluator_exception.h"

cminus::evaluator::exception::base::~base() = default;

std::shared_ptr<cminus::memory::reference> cminus::evaluator::exception::base::create_value() const{
	switch (get_code()){
	case code::invalid_type:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::invalid_type_in_expression));
	case code::inferred_type:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::type_inference_failure));
	case code::incompatible_rval:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::incompatible_right_operand));
	case code::rval_assignment:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::rval_assignment));
	case code::const_assignment:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::constant_assignment));
	case code::rval_ref_assignment:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::rval_ref_assignment));
	case code::const_ref_assignment:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::constant_ref_assignment));
	case code::unsupported_op:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::unsupported_operator));
	default:
		break;
	}

	return nullptr;
}

cminus::evaluator::exception::unnamed::unnamed(code code)
	: base("Unknown evaluator error"), code_(code){}

cminus::evaluator::exception::unnamed::unnamed(code code, const char *message)
	: base(message), code_(code){}

cminus::evaluator::exception::unnamed::unnamed(code code, const std::string &message)
	: base(""), code_(code), message_(message){}

cminus::evaluator::exception::unnamed::~unnamed() = default;

const char *cminus::evaluator::exception::unnamed::what() const{
	return (message_.empty() ? base::what() : message_.data());
}

cminus::evaluator::exception::code cminus::evaluator::exception::unnamed::get_code() const{
	return code_;
}

cminus::evaluator::exception::invalid_type::invalid_type()
	: base("An invalid was encountered in an expression"){}

cminus::evaluator::exception::invalid_type::~invalid_type() = default;

cminus::evaluator::exception::code cminus::evaluator::exception::invalid_type::get_code() const{
	return code::invalid_type;
}

cminus::evaluator::exception::inferred_type::inferred_type()
	: base("Assignment target is an inferred type. Failed to infer type from initialization"){}

cminus::evaluator::exception::inferred_type::~inferred_type() = default;

cminus::evaluator::exception::code cminus::evaluator::exception::inferred_type::get_code() const{
	return code::inferred_type;
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
	: base("Cannot assign value to a constant target"){}

cminus::evaluator::exception::const_assignment::~const_assignment() = default;

cminus::evaluator::exception::code cminus::evaluator::exception::const_assignment::get_code() const{
	return code::const_assignment;
}

cminus::evaluator::exception::rval_ref_assignment::rval_ref_assignment()
	: base("Ref assignment requires an l-value value"){}

cminus::evaluator::exception::rval_ref_assignment::~rval_ref_assignment() = default;

cminus::evaluator::exception::code cminus::evaluator::exception::rval_ref_assignment::get_code() const{
	return code::rval_ref_assignment;
}

cminus::evaluator::exception::const_ref_assignment::const_ref_assignment()
	: base("Cannot assign constant ref value to a non-constant target"){}

cminus::evaluator::exception::const_ref_assignment::~const_ref_assignment() = default;

cminus::evaluator::exception::code cminus::evaluator::exception::const_ref_assignment::get_code() const{
	return code::const_ref_assignment;
}

cminus::evaluator::exception::unsupported_op::unsupported_op()
	: base("The operator does not take specified operand(s)"){}

cminus::evaluator::exception::unsupported_op::~unsupported_op() = default;

cminus::evaluator::exception::code cminus::evaluator::exception::unsupported_op::get_code() const{
	return code::unsupported_op;
}
