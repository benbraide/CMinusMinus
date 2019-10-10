#pragma once

#include "../storage/storage_object.h"
#include "../declaration/declaration_object.h"

namespace cminus::type{
	class enum_ : public type::object, public storage::unnamed_object{
	public:
		using type_base = type::object;
		using storage_base = storage::unnamed_object;

		enum_(const std::string &name, storage_base *parent);

		virtual ~enum_();

		virtual const std::string &get_name() const override;

		virtual storage::object *get_parent() const override;

		virtual std::size_t get_size() const override;

		virtual bool is_exact(const type_base &target) const override;

		virtual int get_score(const type_base &target) const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<type_base> target_type, cast_type type) const override;

		virtual std::shared_ptr<evaluator::object> get_evaluator() const override;

		virtual bool is(query_type type, const type_base *arg = nullptr) const override;

		virtual void add(std::shared_ptr<declaration::object> entry, std::size_t address) override;

		virtual void add(const std::string &name) override;

		virtual void add_entry(const std::string &name, std::shared_ptr<memory::reference> value, bool check_existing = true) override;

		virtual std::shared_ptr<memory::reference> get_item(std::size_t index) const;

		virtual std::size_t get_item_index(const std::string &name) const;

		virtual void compile();

	protected:
		virtual void construct_(std::shared_ptr<memory::reference> target, const std::list<std::shared_ptr<memory::reference>> &args) const override;

		virtual void del_(const std::string &name) override;

		virtual bool exists_(const std::string &name, entry_type type) const override;

		virtual std::shared_ptr<memory::reference> find_(const std::string &name) const override;

		virtual void add_(const std::string &name);

		virtual void compile_();

		template <typename target_type, typename memory_type>
		void compile_(memory_type &memory){
			size_ = sizeof(target_type);

			auto block = runtime::object::memory_object->allocate_write_protected_block(size_);
			if (block == nullptr || (address_ = block->get_address()) == 0u)
				throw memory::exception::allocation_failure();

			std::size_t index = 0u;
			runtime::value_guard guard(runtime::object::is_system, true);

			for (auto &item : items_){
				memory.write_scalar((address_ + (index * size_)), index);
				items_map_[item] = (address_ + (index * size_));
				++index;
			}
		}

		std::list<std::string> items_;
		std::unordered_map<std::string, std::size_t> items_map_;

		std::size_t size_ = 0u;
		std::size_t address_ = 0u;
	};
}
