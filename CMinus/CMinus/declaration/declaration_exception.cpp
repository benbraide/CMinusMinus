#include "declaration_exception.h"

cminus::declaration::exception::unnamed::unnamed(code code)
	: base("Unknown evaluator error"), code_(code){}

cminus::declaration::exception::unnamed::~unnamed() = default;

cminus::declaration::exception::code cminus::declaration::exception::unnamed::get_code() const{
	return code_;
}

cminus::declaration::exception::initialization_required::initialization_required()
	: base("Declaration must be initialized"){}

cminus::declaration::exception::initialization_required::~initialization_required() = default;

cminus::declaration::exception::code cminus::declaration::exception::initialization_required::get_code() const{
	return code::initialization_required;
}
