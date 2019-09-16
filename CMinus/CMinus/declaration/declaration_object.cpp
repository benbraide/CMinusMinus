#include "declaration_object.h"

cminus::declaration::object::~object() = default;

const std::string &cminus::declaration::object::get_name() const{
	return name_;
}

std::shared_ptr<cminus::type::object> cminus::declaration::object::get_type() const{
	return type_;
}

const cminus::attribute::collection &cminus::declaration::object::get_attributes() const{
	return attributes_;
}

cminus::attribute::collection &cminus::declaration::object::get_attributes(){
	return attributes_;
}

unsigned int cminus::declaration::object::get_flags() const{
	return flags_;
}

bool cminus::declaration::object::is(unsigned int flag, bool all) const{
	return (all ? ((flags_ & flag) == flag) : ((flags_ & flag) != 0u));
}
