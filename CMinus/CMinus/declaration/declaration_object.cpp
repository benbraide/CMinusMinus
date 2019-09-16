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
