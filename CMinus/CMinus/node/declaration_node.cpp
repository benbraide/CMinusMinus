#include "declaration_node.h"

cminus::node::variable_declaration::variable_declaration(std::shared_ptr<declaration::variable> value)
	: value_(value){}

cminus::node::variable_declaration::~variable_declaration() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::variable_declaration::evaluate() const{
	value_->evaluate(0u);
	return nullptr;
}

std::shared_ptr<cminus::declaration::variable> cminus::node::variable_declaration::get_decl_value() const{
	return value_;
}
