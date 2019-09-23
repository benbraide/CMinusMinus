#include "node_object.h"

cminus::node::object::~object() = default;

std::size_t cminus::node::object::get_list_count() const{
	return 1u;
}

void cminus::node::object::traverse_list(const std::function<void(const object &)> &callback) const{
	callback(*this);
}
