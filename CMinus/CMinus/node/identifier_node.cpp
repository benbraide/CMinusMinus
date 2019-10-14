#include "../type/class_type.h"
#include "../storage/global_storage.h"

#include "identifier_node.h"

cminus::node::identifier::identifier(const std::string &value)
	: value_(value){}

cminus::node::identifier::~identifier() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::identifier::evaluate() const{
	return get_storage_(true)->find(value_, should_search_tree_());
}

std::shared_ptr<cminus::memory::reference> cminus::node::identifier::lookup(std::shared_ptr<memory::reference> context, bool search_tree) const{
	auto storage = get_storage_(search_tree);
	if (auto class_storage = dynamic_cast<type::class_ *>(storage); class_storage != nullptr)
		return class_storage->find(value_, context, (search_tree && should_search_tree_()));

	return storage->find(value_, (search_tree && should_search_tree_()));
}

std::string cminus::node::identifier::evaluate_as_name() const{
	return value_;
}

std::shared_ptr<cminus::type::object> cminus::node::identifier::evaluate_as_type(bool search_tree) const{
	return get_storage_(search_tree)->find_type(value_, (search_tree && should_search_tree_()));
}

cminus::storage::object *cminus::node::identifier::evaluate_as_storage(bool search_tree) const{
	return get_storage_(search_tree)->find_storage(value_, (search_tree && should_search_tree_()));
}

cminus::storage::object *cminus::node::identifier::get_storage_(bool search_tree) const{
	return runtime::object::current_storage;
}

bool cminus::node::identifier::should_search_tree_() const{
	return true;
}

cminus::node::scoped_identifier::scoped_identifier(std::shared_ptr<object> left, const std::string &value)
	: identifier(value), left_(left){}

cminus::node::scoped_identifier::~scoped_identifier() = default;

std::string cminus::node::scoped_identifier::evaluate_as_name() const{
	return "";
}

cminus::storage::object *cminus::node::scoped_identifier::get_storage_(bool search_tree) const{
	if (left_ == nullptr)
		return runtime::object::global_storage;

	auto storage = left_->evaluate_as_storage(search_tree);
	if (storage == nullptr)
		throw runtime::exception::bad_scope_left();

	return storage;
}

bool cminus::node::scoped_identifier::should_search_tree_() const{
	return false;
}
