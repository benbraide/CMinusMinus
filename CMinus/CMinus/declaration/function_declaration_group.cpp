#include "../node/node_object.h"

#include "function_declaration_group.h"

cminus::declaration::function_group::function_group(const std::string &name, storage::object *parent, std::size_t address)
	: name_(name), parent_(parent), address_(address){}

cminus::declaration::function_group::~function_group() = default;

const std::string &cminus::declaration::function_group::get_name() const{
	return name_;
}

cminus::storage::object *cminus::declaration::function_group::get_parent() const{
	return parent_;
}

std::size_t cminus::declaration::function_group::get_address() const{
	return address_;
}

void cminus::declaration::function_group::add(std::shared_ptr<function_base> entry){
	auto entry_type = entry->get_type();
	if (entry_type == nullptr)//Type required
		return;

	auto it = find_(*entry_type);
	if (it != entries_.end()){//New entry
		entries_[entry.get()] = entry;
		return;
	}

	auto existing_entry_parent = it->first->get_parent(), entry_parent = entry->get_parent();
	if (existing_entry_parent != entry_parent){
		if (existing_entry_parent == parent_ || existing_entry_parent == nullptr || entry_parent == nullptr)
			return;//Existing takes priority

		if (entry_parent == parent_/* || existing_entry_parent->is(type::object::query_type::offspring_of, *entry_parent)*/){//New takes priority
			entries_.erase(it);
			entries_[entry.get()] = entry;
			return;
		}
	}
	else if (auto existing_entry_is_defined = it->first->is_defined(); !existing_entry_is_defined && entry->is_defined())
		it->first->define(entry->get_body());
	else if (existing_entry_is_defined)
		throw exception::function_redefinition();
	else
		throw exception::function_redeclaration();
}

std::shared_ptr<cminus::declaration::function_base> cminus::declaration::function_group::find(const type::object &target_type) const{
	if (auto it = find_(target_type); it != entries_.end())
		return it->second;
	return nullptr;
}

std::shared_ptr<cminus::declaration::function_base> cminus::declaration::function_group::find(const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t *count) const{
	return nullptr;
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::function_group::call(const std::vector<std::shared_ptr<memory::reference>> &args) const{
	return nullptr;
}

cminus::declaration::function_group::map_type::const_iterator cminus::declaration::function_group::find_(const type::object &target_type) const{
	for (auto it = entries_.begin(); it != entries_.end(); ++it){
		if (it->first->get_type()->is(type::object::query_type::exact_parameter_types, &target_type))
			return it;
	}

	return entries_.end();
}
