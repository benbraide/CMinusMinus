#include "../type/system_enum_types.h"
#include "../storage/global_storage.h"

#include "declaration_exception.h"

cminus::declaration::exception::base::~base() = default;

std::shared_ptr<cminus::memory::reference> cminus::declaration::exception::base::create_value() const{
	switch (get_code()){
	case code::bad_declaration:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::bad_declaration));
	case code::initialization_required:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::declaration_initialization_required));
	case code::function_redefinition:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::function_redefinition));
	case code::function_redeclaration:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::function_redeclaration));
	case code::function_expected:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::function_expected));
	case code::constructor_expected:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::constructor_expected));
	case code::destructor_expected:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::destructor_expected));
	case code::function_not_found:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::function_not_found));
	case code::function_not_defined:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::function_not_defined));
	case code::ambiguous_function_call:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::ambiguous_function_call));
	case code::bad_parameter_list:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::bad_parameter_list));
	case code::bad_init_list:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::bad_initialization_list));
	case code::void_function_value_return:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::void_function_value_return));
	case code::value_function_no_return:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::value_function_no_return));
	case code::deleted_function_call:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::deleted_function_call));
	case code::deleted_constructor_call:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::deleted_constructor_call));
	case code::deleted_destructor_call:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::deleted_destructor_call));
	case code::deleted_operator_call:
		return runtime::object::global_storage->get_enum_value(storage::global::cached_type::exception, static_cast<std::size_t>(type::exception::code_type::deleted_operator_call));
	default:
		break;
	}

	return nullptr;
}

cminus::declaration::exception::unnamed::unnamed(code code)
	: unnamed(code, "Unknown evaluator error"){}

cminus::declaration::exception::unnamed::unnamed(code code, const char *message)
	: base(message), code_(code){}

cminus::declaration::exception::unnamed::unnamed(code code, const std::string &message)
	: base(""), code_(code), message_(message){}

cminus::declaration::exception::unnamed::~unnamed() = default;

const char *cminus::declaration::exception::unnamed::what() const{
	return (message_.empty() ? base::what() : message_.data());
}

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

cminus::declaration::exception::deleted_function_call::deleted_function_call()
	: base("Cannot call a deleted function"){}

cminus::declaration::exception::deleted_function_call::~deleted_function_call() = default;

cminus::declaration::exception::code cminus::declaration::exception::deleted_function_call::get_code() const{
	return code::deleted_function_call;
}

cminus::declaration::exception::deleted_constructor_call::deleted_constructor_call()
	: base("Cannot call a deleted constructor"){}

cminus::declaration::exception::deleted_constructor_call::~deleted_constructor_call() = default;

cminus::declaration::exception::code cminus::declaration::exception::deleted_constructor_call::get_code() const{
	return code::deleted_constructor_call;
}

cminus::declaration::exception::deleted_destructor_call::deleted_destructor_call()
	: base("Cannot call a deleted destructor"){}

cminus::declaration::exception::deleted_destructor_call::~deleted_destructor_call() = default;

cminus::declaration::exception::code cminus::declaration::exception::deleted_destructor_call::get_code() const{
	return code::deleted_destructor_call;
}

cminus::declaration::exception::deleted_operator_call::deleted_operator_call()
	: base("Cannot call a deleted operator"){}

cminus::declaration::exception::deleted_operator_call::~deleted_operator_call() = default;

cminus::declaration::exception::code cminus::declaration::exception::deleted_operator_call::get_code() const{
	return code::deleted_operator_call;
}
