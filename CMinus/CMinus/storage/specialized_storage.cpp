#include "specialized_storage.h"

cminus::storage::class_member::class_member(const declaration::function_base &owner, std::shared_ptr<memory::reference> context)
	: owner_(owner), context_(context){}

cminus::storage::class_member::~class_member(){
	destroy_entries_();
}

const std::string &cminus::storage::class_member::get_name() const{
	return owner_.get_name();
}

cminus::storage::object *cminus::storage::class_member::get_parent() const{
	return owner_.get_parent();
}

std::shared_ptr<cminus::memory::reference> cminus::storage::class_member::get_context() const{
	return context_;
}

const cminus::declaration::function_base &cminus::storage::class_member::get_owner() const{
	return owner_;
}
