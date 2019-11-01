#include "../type/class_type.h"
#include "../node/node_object.h"
#include "../storage/global_storage.h"

#include "function_declaration_group.h"

cminus::declaration::function_group::function_group(callable::id_type id, const std::string &name, storage::object *parent, std::size_t address)
	: callable_group(name, runtime::object::global_storage->get_cached_type(storage::global::cached_type::function)), id_(id), parent_(parent), address_(address){}

cminus::declaration::function_group::~function_group() = default;

cminus::declaration::callable::id_type cminus::declaration::function_group::get_id() const{
	return id_;
}

cminus::storage::object *cminus::declaration::function_group::get_parent() const{
	return parent_;
}

void cminus::declaration::function_group::set_address(std::size_t value){
	if (address_ == 0u)
		address_ = value;
}

std::size_t cminus::declaration::function_group::get_address() const{
	return address_;
}

std::shared_ptr<cminus::declaration::callable> cminus::declaration::function_group::find(const type::function &target_type) const{
	if (auto it = find_(target_type); it != entries_.end())
		return *it;
	return nullptr;
}

std::shared_ptr<cminus::declaration::callable> cminus::declaration::function_group::find(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const{
	return find_(context, args, required_size);
}

std::shared_ptr<cminus::declaration::callable> cminus::declaration::function_group::find(std::shared_ptr<memory::reference> context, const std::vector<callable::arg_info> &args, std::size_t required_size) const{
	return find_(context, args, required_size);
}

std::shared_ptr<cminus::declaration::callable> cminus::declaration::function_group::find_by_args(const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const{
	return find_by_args_(args, required_size);
}

std::shared_ptr<cminus::declaration::callable> cminus::declaration::function_group::find_by_args(const std::vector<callable::arg_info> &args, std::size_t required_size) const{
	return find_by_args_(args, required_size);
}

void cminus::declaration::function_group::traverse_list(const std::function<void(std::shared_ptr<callable>)> &callback) const{
	for (auto &entry : entries_)
		callback(entry);
}

std::size_t cminus::declaration::function_group::get_count() const{
	return entries_.size();
}

void cminus::declaration::function_group::add_(std::shared_ptr<callable> entry){
	auto entry_type = entry->get_type();
	if (entry_type == nullptr)//Type required
		return;

	auto it = find_(*entry_type->as<type::function>());
	if (it == entries_.end()){//New entry
		entries_.push_back(entry);
		return;
	}

	auto existing_entry_parent = (*it)->get_parent(), entry_parent = entry->get_parent();
	if (existing_entry_parent != entry_parent){
		if (existing_entry_parent == parent_ || existing_entry_parent == nullptr || entry_parent == nullptr)
			return;//Existing takes priority

		auto existing_entry_type_parent = dynamic_cast<type::class_ *>(existing_entry_parent), entry_type_parent = dynamic_cast<type::class_ *>(entry_parent);
		if (entry_parent == parent_ || existing_entry_type_parent->compute_base_offset(*entry_type_parent) == static_cast<std::size_t>(-1)){//New takes priority
			entries_.insert(it, entry);
			entries_.erase(++it);
		}
	}
	else if (auto existing_entry_is_defined = (*it)->is_defined(); !existing_entry_is_defined && entry->is_defined())
		(*it)->define(entry->get_definition());
	else if (existing_entry_is_defined)
		throw exception::function_redefinition();
	else
		throw exception::function_redeclaration();
}

cminus::declaration::function_group::list_type::const_iterator cminus::declaration::function_group::find_(const type::function &target_type) const{
	for (auto it = entries_.begin(); it != entries_.end(); ++it){
		if ((*it)->get_type()->as<type::function>()->is_exact_parameter_types(target_type))
			return it;
	}

	return entries_.end();
}
