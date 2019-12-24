#include "../declaration/variable_declaration.h"
#include "../declaration/function_declaration_group.h"

#include "global_storage.h"
#include "compiled_storage.h"

cminus::storage::compiled::~compiled(){
	destroy_entries_();
}

void cminus::storage::compiled::add_variable_(std::shared_ptr<declaration::variable> entry, std::size_t address){
	if (should_compile_){
		auto &name = entry->get_name();
		if (name.empty())
			throw storage::exception::unnamed_entry();

		auto entry_memory_size = entry->get_type()->get_memory_size();
		if (entry_memory_size == 0u)
			throw declaration::exception::bad_declaration();

		entries_.push_back(entry_info{ entry, static_size_ });
		mapped_entries_[name] = &entries_.back();
		static_size_ += entry_memory_size;
	}
	else
		named_object::add_variable_(entry, address);
}

void cminus::storage::compiled::add_callable_(std::shared_ptr<declaration::callable> entry, std::size_t address){
	if (should_compile_){
		auto &name = entry->get_name();
		if (auto it = mapped_entries_.find(name); it != mapped_entries_.end()){
			if (auto group = dynamic_cast<declaration::callable_group *>(it->second->decl.get()); group != nullptr)
				group->add(entry);
			else
				throw storage::exception::duplicate_entry();
		}
		else if (auto group = std::make_shared<declaration::function_group>(entry->get_id(), name, this, 0u); group != nullptr){
			group->add(entry);
			entries_.push_back(entry_info{ group, group.get() });
			mapped_entries_[name] = &entries_.back();
			function_size_ += sizeof(void *);
		}
		else
			throw memory::exception::allocation_failure();
	}
	else
		named_object::add_callable_(entry, address);
}

void cminus::storage::compiled::compile_(){
	should_compile_ = false;
	if ((static_size_ + function_size_) == 0u)
		return;

	if (static_size_ != 0u)
		static_address_ = runtime::object::memory_object->allocate_block(static_size_);

	if (function_size_ != 0u)
		function_address_ = runtime::object::memory_object->allocate_write_protected_block(function_size_);

	declaration::variable *var_decl;
	declaration::callable_group *func_decl = nullptr;

	auto static_address = static_address_, function_address = function_address_;
	for (auto &decl : mapped_entries_){//Initialize static members
		if ((func_decl = dynamic_cast<declaration::callable_group *>(decl.second->decl.get())) != nullptr){
			runtime::value_guard guard(runtime::object::state, (runtime::object::state | runtime::flags::system));
			runtime::object::memory_object->write_scalar(function_address, func_decl);

			func_decl->set_address(function_address);
			function_address += sizeof(void *);
		}
		else if ((var_decl = dynamic_cast<declaration::variable *>(decl.second->decl.get())) != nullptr && decl.second->decl->is(declaration::flags::static_)){
			decl.second->value = var_decl->evaluate(static_address);
			static_address += var_decl->get_type()->get_memory_size();
		}
	}
}
