#include "../type/class_type.h"
#include "../node/node_object.h"
#include "../storage/global_storage.h"

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

std::shared_ptr<cminus::type::object> cminus::declaration::function_group::get_type() const{
	switch (entries_.size()){
	case 0u:
		return runtime::object::global_storage->get_cached_type(storage::global::cached_type::undefined);
	case 1u:
		return entries_.begin()->first->get_type();
	default:
		break;
	}

	return runtime::object::global_storage->get_cached_type(storage::global::cached_type::function);
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

		auto existing_entry_type_parent = dynamic_cast<type::object *>(existing_entry_parent), entry_type_parent = dynamic_cast<type::object *>(entry_parent);
		if (entry_parent == parent_ || existing_entry_type_parent->compute_base_offset(*entry_type_parent) == static_cast<std::size_t>(-1)){//New takes priority
			entries_.erase(it);
			entries_[entry.get()] = entry;
			return;
		}
	}
	else if (auto existing_entry_is_defined = it->first->is_defined(); !existing_entry_is_defined && entry->is_defined())
		it->first->define(entry->get_definition());
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

std::shared_ptr<cminus::declaration::function_base> cminus::declaration::function_group::find(const std::list<std::shared_ptr<memory::reference>> &args, std::size_t *count) const{
	auto highest_rank_score = type::object::get_score_value(type::object::score_result_type::nil), current_rank_score = highest_rank_score;

	std::size_t match_count = 0u;
	std::shared_ptr<function_base> highest_ranked;

	for (auto &entry : entries_){
		if (highest_rank_score < (current_rank_score = entry.first->get_score(args))){
			highest_rank_score = current_rank_score;
			highest_ranked = entry.second;
			match_count = 1u;
		}
		else if (highest_rank_score == current_rank_score)
			++match_count;
	}

	if (count != nullptr)//Update count
		*count = ((highest_ranked == nullptr) ? 0u : match_count);

	return highest_ranked;
}

std::shared_ptr<cminus::memory::reference> cminus::declaration::function_group::call(const std::list<std::shared_ptr<memory::reference>> &args) const{
	std::size_t count = 0;
	auto entry = find(args, &count);

	if (count == 0u || entry == nullptr)
		throw exception::function_not_found();

	if (1u < count)
		throw exception::ambiguous_function_call();

	if (auto class_parent = dynamic_cast<type::class_ *>(entry->get_parent()); class_parent != nullptr && !class_parent->is_accessible(entry->get_flags()))
		throw storage::exception::inaccessible_entry();

	return entry->call_(args);
}

cminus::declaration::function_group::map_type::const_iterator cminus::declaration::function_group::find_(const type::object &target_type) const{
	for (auto it = entries_.begin(); it != entries_.end(); ++it){
		if (it->first->get_type()->is(type::object::query_type::exact_parameter_types, &target_type))
			return it;
	}

	return entries_.end();
}
