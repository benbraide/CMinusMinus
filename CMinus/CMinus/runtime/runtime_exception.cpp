#include "runtime_exception.h"

#include "../memory/memory_reference.h"

cminus::runtime::exception::unnamed::unnamed(code code)
	: base("Unknown evaluator error"), code_(code){}

cminus::runtime::exception::unnamed::~unnamed() = default;

cminus::runtime::exception::code cminus::runtime::exception::unnamed::get_code() const{
	return code_;
}

cminus::runtime::exception::return_interrupt::return_interrupt(std::shared_ptr<memory::reference> value)
	: base("Return statement must be inside a function context"){}

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
