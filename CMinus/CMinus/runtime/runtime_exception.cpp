#include "runtime_exception.h"

#include "../memory/memory_reference.h"

cminus::runtime::exception::unnamed::unnamed(code code)
	: base("Unknown evaluator error"), code_(code){}

cminus::runtime::exception::unnamed::unnamed(code code, const char *message)
	: base(message), code_(code){}

cminus::runtime::exception::unnamed::unnamed(code code, const std::string &message)
	: base(""), code_(code), message_(message){}

cminus::runtime::exception::unnamed::~unnamed() = default;

const char *cminus::runtime::exception::unnamed::what() const{
	return (message_.empty() ? base::what() : message_.data());
}

cminus::runtime::exception::code cminus::runtime::exception::unnamed::get_code() const{
	return code_;
}

cminus::runtime::exception::return_interrupt::return_interrupt(std::shared_ptr<memory::reference> value)
	: base("Return statement must be inside a function context"), value_(value){}

cminus::runtime::exception::return_interrupt::~return_interrupt() = default;

cminus::runtime::exception::code cminus::runtime::exception::return_interrupt::get_code() const{
	return code::return_interrupt;
}

std::shared_ptr<cminus::memory::reference> cminus::runtime::exception::return_interrupt::get_value() const{
	return value_;
}

cminus::runtime::exception::void_return_interrupt::void_return_interrupt()
	: return_interrupt(nullptr){}

cminus::runtime::exception::void_return_interrupt::~void_return_interrupt() = default;

cminus::runtime::exception::control_interrupt::control_interrupt(value_type value)
	: base("'break' or 'continue' statement must be inside a iteration or switch context"), value_(value){}

cminus::runtime::exception::control_interrupt::~control_interrupt() = default;

cminus::runtime::exception::code cminus::runtime::exception::control_interrupt::get_code() const{
	return code::control_interrupt;
}

cminus::runtime::exception::control_interrupt::value_type cminus::runtime::exception::control_interrupt::get_value() const{
	return value_;
}

cminus::runtime::exception::bad_constructor::bad_constructor()
	: base("Constructor is ill-formed"){}

cminus::runtime::exception::bad_constructor::~bad_constructor() = default;

cminus::runtime::exception::code cminus::runtime::exception::bad_constructor::get_code() const{
	return code::bad_constructor;
}

cminus::runtime::exception::bad_destructor::bad_destructor()
	: base("Destructor is ill-formed"){}

cminus::runtime::exception::bad_destructor::~bad_destructor() = default;

cminus::runtime::exception::code cminus::runtime::exception::bad_destructor::get_code() const{
	return code::bad_destructor;
}

cminus::runtime::exception::bad_constructor_init_list::bad_constructor_init_list()
	: base("Constructor initialization list is ill-formed"){}

cminus::runtime::exception::bad_constructor_init_list::~bad_constructor_init_list() = default;

cminus::runtime::exception::code cminus::runtime::exception::bad_constructor_init_list::get_code() const{
	return code::bad_constructor_init_list;
}

cminus::runtime::exception::bad_scope_left::bad_scope_left()
	: base("Name left of scope resolution operator is not a storage"){}

cminus::runtime::exception::bad_scope_left::~bad_scope_left() = default;

cminus::runtime::exception::code cminus::runtime::exception::bad_scope_left::get_code() const{
	return code::bad_scope_left;
}

cminus::runtime::exception::bad_member_access_left::bad_member_access_left()
	: base("Object left of member access operator is not a class instance"){}

cminus::runtime::exception::bad_member_access_left::~bad_member_access_left() = default;

cminus::runtime::exception::code cminus::runtime::exception::bad_member_access_left::get_code() const{
	return code::bad_member_access_left;
}

cminus::runtime::exception::bad_pointer_member_access_left::bad_pointer_member_access_left()
	: base("Object left of pointer member access operator is not a pointer to a class instance"){}

cminus::runtime::exception::bad_pointer_member_access_left::~bad_pointer_member_access_left() = default;

cminus::runtime::exception::code cminus::runtime::exception::bad_pointer_member_access_left::get_code() const{
	return code::bad_pointer_member_access_left;
}

cminus::runtime::exception::bad_control_condition::bad_control_condition()
	: base("Control condition is required to evaluate to a boolean value"){}

cminus::runtime::exception::bad_control_condition::~bad_control_condition() = default;

cminus::runtime::exception::code cminus::runtime::exception::bad_control_condition::get_code() const{
	return code::bad_control_condition;
}

cminus::runtime::exception::out_of_range::out_of_range()
	: base("Specified index is out of range"){}

cminus::runtime::exception::out_of_range::~out_of_range() = default;

cminus::runtime::exception::code cminus::runtime::exception::out_of_range::get_code() const{
	return code::out_of_range;
}

cminus::runtime::exception::not_supported::not_supported()
	: base("Specified action is not supported"){}

cminus::runtime::exception::not_supported::~not_supported() = default;

cminus::runtime::exception::code cminus::runtime::exception::not_supported::get_code() const{
	return code::not_supported;
}
