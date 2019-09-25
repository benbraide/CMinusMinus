#include "../storage/global_storage.h"

#include "identifier_node.h"

cminus::node::identifier::identifier(const std::string &value)
	: value_(value){}

cminus::node::identifier::~identifier() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::identifier::evaluate() const{
	return get_storage_()->find(value_, should_search_tree_());
}

std::string cminus::node::identifier::evaluate_as_name() const{
	return value_;
}

std::shared_ptr<cminus::type::object> cminus::node::identifier::evaluate_as_type() const{
	return get_storage_()->find_type(value_, should_search_tree_());
}

cminus::storage::object *cminus::node::identifier::evaluate_as_storage() const{
	return get_storage_()->find_storage(value_, should_search_tree_());
}

cminus::storage::object *cminus::node::identifier::get_storage_() const{
	return runtime::object::current_storage;
}

bool cminus::node::identifier::should_search_tree_() const{
	return true;
}

cminus::node::scoped_identifier::scoped_identifier(std::shared_ptr<identifier> left, const std::string &value)
	: identifier(value), left_(left){}

cminus::node::scoped_identifier::~scoped_identifier() = default;

std::string cminus::node::scoped_identifier::evaluate_as_name() const{
	return "";
}

cminus::storage::object *cminus::node::scoped_identifier::get_storage_() const{
	return ((left_ == nullptr) ? runtime::object::global_storage : left_->evaluate_as_storage());
}

bool cminus::node::scoped_identifier::should_search_tree_() const{
	return false;
}
