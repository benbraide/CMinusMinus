#include "../node/node_object.h"
#include "../storage/global_storage.h"

#include "function_declaration_group.h"

cminus::declaration::function_group::function_group(callable::id_type id, const std::string &name, storage::object *parent, std::size_t address)
	: id_(id), name_(name), parent_(parent), address_(address){}

cminus::declaration::function_group::~function_group() = default;

cminus::declaration::callable::id_type cminus::declaration::function_group::get_id() const{
	return id_;
}

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
		return (*entries_.begin())->get_type();
	default:
		break;
	}

	return runtime::object::global_storage->get_cached_type(storage::global::cached_type::function);
}

std::shared_ptr<cminus::declaration::callable> cminus::declaration::function_group::find(const type::object &target_type) const{
	if (auto it = find_(target_type); it != entries_.end())
		return *it;
	return nullptr;
}

std::shared_ptr<cminus::declaration::callable> cminus::declaration::function_group::find(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args) const{
	auto highest_rank_score = type::object::get_score_value(type::object::score_result_type::nil), current_rank_score = highest_rank_score;

	std::size_t match_count = 0u;
	std::shared_ptr<callable> highest_ranked;

	for (auto entry : entries_){
		if (highest_rank_score < (current_rank_score = entry->get_score(context, args))){
			highest_rank_score = current_rank_score;
			highest_ranked = entry;
			match_count = 1u;
		}
		else if (highest_rank_score == current_rank_score)
			++match_count;
	}

	if (1u < match_count && highest_ranked != nullptr)
		throw exception::ambiguous_function_call();

	return highest_ranked;
}

void cminus::declaration::function_group::traverse_list(const std::function<void(std::shared_ptr<callable>)> &callback){
	for (auto entry : entries_)
		callback(entry);
}

void cminus::declaration::function_group::add_(std::shared_ptr<callable> entry){
	auto entry_type = entry->get_type();
	if (entry_type == nullptr)//Type required
		return;

	auto it = find_(*entry_type);
	if (it == entries_.end()){//New entry
		entries_.push_back(entry);
		return;
	}

	auto existing_entry_parent = (*it)->get_parent(), entry_parent = entry->get_parent();
	if (existing_entry_parent != entry_parent){
		if (existing_entry_parent == parent_ || existing_entry_parent == nullptr || entry_parent == nullptr)
			return;//Existing takes priority

		auto existing_entry_type_parent = dynamic_cast<type::object *>(existing_entry_parent), entry_type_parent = dynamic_cast<type::object *>(entry_parent);
		/*if (entry_parent == parent_ || existing_entry_type_parent->compute_base_offset(*entry_type_parent) == static_cast<std::size_t>(-1)){//New takes priority
			entries_.erase(it);
			entries_.push_back(entry);
			return;
		}*/
	}
	else if (auto existing_entry_is_defined = (*it)->is_defined(); !existing_entry_is_defined && entry->is_defined())
		(*it)->define(entry->get_definition());
	else if (existing_entry_is_defined)
		throw exception::function_redefinition();
	else
		throw exception::function_redeclaration();
}

cminus::declaration::function_group::list_type::const_iterator cminus::declaration::function_group::find_(const type::object &target_type) const{
	/*for (auto it = entries_.begin(); it != entries_.end(); ++it){
		if ((*it)->get_type()->is(type::object::query_type::exact_parameter_types, &target_type))
			return it;
	}*/

	return entries_.end();
}
