#pragma once

#include "../node/node_object.h"
#include "../storage/global_storage.h"

#include "function_declaration_group_base.h"

namespace cminus::declaration{
	template <class base_type>
	class generic_function_group : public base_type{
	public:
		using map_type = std::unordered_map<function_base *, std::shared_ptr<function_base>>;

		generic_function_group(const std::string &name, storage::object *parent, std::size_t address)
			: name_(name), parent_(parent), address_(address){}

		virtual ~generic_function_group() = default;

		virtual const std::string &get_name() const override{
			return name_;
		}

		virtual storage::object *get_parent() const override{
			return parent_;
		}

		virtual std::size_t get_address() const override{
			return address_;
		}

		virtual std::shared_ptr<type::object> get_type() const override{
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

		virtual std::shared_ptr<function_base> find(const type::object &target_type) const override{
			if (auto it = find_(target_type); it != entries_.end())
				return it->second;
			return nullptr;
		}

		virtual std::shared_ptr<function_base> find(const std::list<std::shared_ptr<memory::reference>> &args, std::size_t *count = nullptr) const override{
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

		virtual std::shared_ptr<memory::reference> call(const std::list<std::shared_ptr<memory::reference>> &args) const override{
			std::size_t count = 0;
			auto entry = find(args, &count);

			if (count == 0u || entry == nullptr)
				throw exception::function_not_found();

			if (1u < count)
				throw exception::ambiguous_function_call();

			if (auto parent = entry->get_parent(); parent != nullptr && !parent->is_accessible(entry->get_flags()))
				throw storage::exception::inaccessible_entry();

			return entry->call_(args);
		}

	protected:
		virtual void add_(std::shared_ptr<function_base> entry) override{
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

		virtual map_type::const_iterator find_(const type::object &target_type) const{
			for (auto it = entries_.begin(); it != entries_.end(); ++it){
				if (it->first->get_type()->is(type::object::query_type::exact_parameter_types, &target_type))
					return it;
			}

			return entries_.end();
		}

		std::string name_;
		storage::object *parent_;
		std::size_t address_;
		map_type entries_;
	};

	using function_group = generic_function_group<function_group_base>;
	using constructor_group = generic_function_group<constructor_group_base>;
	using destructor_group = generic_function_group<destructor_group_base>;
}
