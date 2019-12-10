#include "declaration_node.h"

cminus::node::variable_declaration::variable_declaration(const std::string &name, std::shared_ptr<object> type, std::shared_ptr<object> attributes, unsigned int flags, std::shared_ptr<object> initialization)
	: name_(name), type_(type), attributes_(attributes), flags_(flags), initialization_(initialization){}

cminus::node::variable_declaration::~variable_declaration() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::variable_declaration::evaluate() const{
	return get_decl_value()->evaluate(0u);
}

std::shared_ptr<cminus::declaration::variable> cminus::node::variable_declaration::get_decl_value() const{
	if (value_ == nullptr){
		value_ = std::make_shared<declaration::variable>(
			name_,
			type_->evaluate_as_type(true),
			attribute::collection::list_type{},
			flags_,
			initialization_
		);
	}

	return value_;
}
