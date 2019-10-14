#include "../type/class_type.h"
#include "../storage/global_storage.h"

#include "node_object.h"

cminus::node::object::~object() = default;

std::size_t cminus::node::object::get_list_count() const{
	return 1u;
}

void cminus::node::object::traverse_list(const std::function<void(const object &)> &callback) const{
	callback(*this);
}

std::shared_ptr<cminus::memory::reference> cminus::node::object::lookup(std::shared_ptr<memory::reference> context, bool search_tree) const{
	auto name = evaluate_as_name();
	if (name.empty())
		return nullptr;

	auto class_storage = dynamic_cast<type::class_ *>(runtime::object::current_storage);
	if (class_storage == nullptr)
		return runtime::object::current_storage->find(name, search_tree);

	return class_storage->find(name, context, search_tree);
}

std::string cminus::node::object::evaluate_as_name() const{
	return "";
}

std::shared_ptr<cminus::type::object> cminus::node::object::evaluate_as_type(bool search_tree) const{
	return nullptr;
}

cminus::storage::object *cminus::node::object::evaluate_as_storage(bool search_tree) const{
	return nullptr;
}
