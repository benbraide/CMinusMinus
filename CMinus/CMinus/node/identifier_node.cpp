#include "../type/class_type.h"
#include "../type/proxy_type.h"

#include "../storage/global_storage.h"
#include "../storage/specialized_storage.h"

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

cminus::node::scoped_identifier::scoped_identifier(std::shared_ptr<object> left, const std::string &value)
	: identifier(value), left_(left){}

cminus::node::scoped_identifier::~scoped_identifier() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::scoped_identifier::evaluate() const{
	auto wrapper_storage = dynamic_cast<storage::class_wrapper *>(runtime::object::current_storage);
	if (wrapper_storage == nullptr)
		return identifier::evaluate();

	auto class_parent = dynamic_cast<type::class_ *>(wrapper_storage->get_parent());
	if (class_parent == nullptr)
		return identifier::evaluate();

	auto storage = dynamic_cast<type::class_ *>(get_storage_());
	if (storage == nullptr)
		throw runtime::exception::bad_scope_left();

	auto offset = class_parent->compute_base_offset(*storage);
	if (offset == static_cast<std::size_t>(-1))
		throw runtime::exception::bad_scope_left();

	auto context = wrapper_storage->get_context();
	if (class_parent != storage)
		context = context->apply_offset(offset, std::make_shared<type::proxy>(*storage));

	return storage->find(value_, context, false);
}

std::string cminus::node::scoped_identifier::evaluate_as_name() const{
	return "";
}

cminus::storage::object *cminus::node::scoped_identifier::get_storage_() const{
	if (left_ == nullptr)
		return runtime::object::global_storage;

	auto storage = left_->evaluate_as_storage();
	if (storage == nullptr)
		throw runtime::exception::bad_scope_left();

	return storage;
}

bool cminus::node::scoped_identifier::should_search_tree_() const{
	return false;
}
