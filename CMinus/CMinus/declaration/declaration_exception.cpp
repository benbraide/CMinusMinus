#include "declaration_exception.h"

cminus::declaration::exception::unnamed::unnamed(code code)
	: base("Unknown evaluator error"), code_(code){}

cminus::declaration::exception::unnamed::~unnamed() = default;

cminus::declaration::exception::code cminus::declaration::exception::unnamed::get_code() const{
	return code_;
}

cminus::declaration::exception::bad_declaration::bad_declaration()
	: base("Declaration is ill-formed"){}

cminus::declaration::exception::bad_declaration::~bad_declaration() = default;

cminus::declaration::exception::code cminus::declaration::exception::bad_declaration::get_code() const{
	return code::bad_declaration;
}

cminus::declaration::exception::initialization_required::initialization_required()
	: base("Declaration must be initialized"){}

cminus::declaration::exception::initialization_required::~initialization_required() = default;

cminus::declaration::exception::code cminus::declaration::exception::initialization_required::get_code() const{
	return code::initialization_required;
}

cminus::declaration::exception::function_redefinition::function_redefinition()
	: base("Function is already defined"){}

cminus::declaration::exception::function_redefinition::~function_redefinition() = default;

cminus::declaration::exception::code cminus::declaration::exception::function_redefinition::get_code() const{
	return code::function_redefinition;
}

cminus::declaration::exception::function_redeclaration::function_redeclaration()
	: base("Function is already declared"){}

cminus::declaration::exception::function_redeclaration::~function_redeclaration() = default;

cminus::declaration::exception::code cminus::declaration::exception::function_redeclaration::get_code() const{
	return code::function_redeclaration;
}

cminus::declaration::exception::function_expected::function_expected()
	: base("Callable object declaration cannot be a constructor or destructor"){}

cminus::declaration::exception::function_expected::~function_expected() = default;

cminus::declaration::exception::code cminus::declaration::exception::function_expected::get_code() const{
	return code::function_expected;
}

cminus::declaration::exception::constructor_expected::constructor_expected()
	: base("Expected a constructor declaration"){}

cminus::declaration::exception::constructor_expected::~constructor_expected() = default;

cminus::declaration::exception::code cminus::declaration::exception::constructor_expected::get_code() const{
	return code::constructor_expected;
}

cminus::declaration::exception::destructor_expected::destructor_expected()
	: base("Expected a destructor declaration"){}

cminus::declaration::exception::destructor_expected::~destructor_expected() = default;

cminus::declaration::exception::code cminus::declaration::exception::destructor_expected::get_code() const{
	return code::destructor_expected;
}

cminus::declaration::exception::function_not_found::function_not_found()
	: base("No function found taking the specified arguments"){}

cminus::declaration::exception::function_not_found::~function_not_found() = default;

cminus::declaration::exception::code cminus::declaration::exception::function_not_found::get_code() const{
	return code::function_not_found;
}

cminus::declaration::exception::function_not_defined::function_not_defined()
	: base("Called function has no definition"){}

cminus::declaration::exception::function_not_defined::~function_not_defined() = default;

cminus::declaration::exception::code cminus::declaration::exception::function_not_defined::get_code() const{
	return code::function_not_defined;
}

cminus::declaration::exception::ambiguous_function_call::ambiguous_function_call()
	: base("Function call is ambiguous"){}

cminus::declaration::exception::ambiguous_function_call::~ambiguous_function_call() = default;

cminus::declaration::exception::code cminus::declaration::exception::ambiguous_function_call::get_code() const{
	return code::ambiguous_function_call;
}

cminus::declaration::exception::bad_parameter_list::bad_parameter_list()
	: base("Function parameter list is ill-formed"){}

cminus::declaration::exception::bad_parameter_list::~bad_parameter_list() = default;

cminus::declaration::exception::code cminus::declaration::exception::bad_parameter_list::get_code() const{
	return code::bad_parameter_list;
}

cminus::declaration::exception::bad_init_list::bad_init_list()
	: base("Constructor initialization list is ill-formed"){}

cminus::declaration::exception::bad_init_list::~bad_init_list() = default;

cminus::declaration::exception::code cminus::declaration::exception::bad_init_list::get_code() const{
	return code::bad_init_list;
}

cminus::declaration::exception::void_function_value_return::void_function_value_return()
	: base("A void function cannot return a value"){}

cminus::declaration::exception::void_function_value_return::~void_function_value_return() = default;

cminus::declaration::exception::code cminus::declaration::exception::void_function_value_return::get_code() const{
	return code::void_function_value_return;
}

cminus::declaration::exception::value_function_no_return::value_function_no_return()
	: base("Function must return a value"){}

cminus::declaration::exception::value_function_no_return::~value_function_no_return() = default;

cminus::declaration::exception::code cminus::declaration::exception::value_function_no_return::get_code() const{
	return code::value_function_no_return;
}

cminus::declaration::exception::invalid_function_return::invalid_function_return()
	: base("Constructor and destructor cannot have return statements"){}

cminus::declaration::exception::invalid_function_return::~invalid_function_return() = default;

cminus::declaration::exception::code cminus::declaration::exception::invalid_function_return::get_code() const{
	return code::invalid_function_return;
}
