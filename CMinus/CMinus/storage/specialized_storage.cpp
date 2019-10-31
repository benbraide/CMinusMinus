#include "../type/class_type.h"
#include "../declaration/variable_declaration.h"

#include "specialized_storage.h"

cminus::storage::class_member::class_member(const declaration::callable &owner)
	: owner_(owner){}

cminus::storage::class_member::~class_member(){
	destroy_entries_();
}

const std::string &cminus::storage::class_member::get_name() const{
	return owner_.get_name();
}

cminus::storage::object *cminus::storage::class_member::get_parent() const{
	return owner_.get_parent();
}

std::shared_ptr<cminus::memory::reference> cminus::storage::class_member::find(const std::string &name, bool search_tree) const{
	if (auto entry = unnamed_object::find(name, false); entry != nullptr || !search_tree)
		return entry;

	auto parent = get_parent();
	if (parent == nullptr)
		return nullptr;

	if (auto class_parent = dynamic_cast<type::class_ *>(parent); class_parent != nullptr)
		return class_parent->find(name, context_, true);

	return parent->find(name, true);
}

void cminus::storage::class_member::set_context(std::shared_ptr<memory::reference> value){
	context_ = value;
}

std::shared_ptr<cminus::memory::reference> cminus::storage::class_member::get_context() const{
	return context_;
}

const cminus::declaration::callable &cminus::storage::class_member::get_owner() const{
	return owner_;
}
