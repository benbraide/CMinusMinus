#include "literal_node.h"

cminus::node::string_literal::string_literal(const std::string &value)
	: value_(value){}

cminus::node::string_literal::~string_literal() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::string_literal::evaluate() const{
	return runtime::object::global_storage->create_string(value_);
}
