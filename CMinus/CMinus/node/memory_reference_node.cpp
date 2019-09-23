#include "memory_reference_node.h"

cminus::node::memory_reference::memory_reference(std::shared_ptr<cminus::memory::reference> value)
	: value_(value){}

cminus::node::memory_reference::~memory_reference() = default;

std::shared_ptr<cminus::memory::reference> cminus::node::memory_reference::evaluate() const{
	return value_;
}

std::shared_ptr<cminus::memory::reference> cminus::node::memory_reference::get_value() const{
	return value_;
}
