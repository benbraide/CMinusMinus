#pragma once

#include "../storage/storage_object.h"
#include "../declaration/declaration_object.h"

#include "primitive_type.h"

namespace cminus::type{
	class enum_ : public primitive, public storage::unnamed_object{
	public:
		using type_base = type::object;
		using storage_base = storage::unnamed_object;

		enum_(const std::string &name, storage_base *parent);

		virtual ~enum_();

		virtual const std::string &get_name() const override;

		virtual std::string get_qname() const override;

		virtual storage::object *get_parent() const override;

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::size_t get_size() const override;

		virtual evaluator::object::id_type get_evaluator_id() const override;

		virtual void add(std::shared_ptr<declaration::object> entry, std::size_t address) override;

		virtual void add_entry(std::shared_ptr<declaration::object> entry, std::shared_ptr<memory::reference> value, bool check_existing = true) override;

		virtual void add(const std::string &name);

		virtual std::shared_ptr<memory::reference> get_item(std::size_t index) const;

		virtual std::size_t get_item_index(const std::string &name) const;

		virtual void compile();

	protected:
		virtual bool exists_(const std::string &name) const override;

		virtual std::shared_ptr<memory::reference> find_(const std::string &name, std::shared_ptr<memory::reference> context, std::size_t address) const override;

		virtual void add_(const std::string &name);

		virtual void compile_();

		template <typename target_type, typename memory_type>
		void compile_(memory_type &memory){
			size_ = sizeof(target_type);

			auto block = runtime::object::memory_object->allocate_write_protected_block(size_);
			if (block == nullptr || (address_ = block->get_address()) == 0u)
				throw memory::exception::allocation_failure();

			std::size_t index = 0u;
			runtime::value_guard guard(runtime::object::state, (runtime::object::state | runtime::flags::system));

			for (auto &item : items_){
				memory.write_scalar((address_ + (index * size_)), index);
				items_map_[item] = (address_ + (index * size_));
				++index;
			}
		}

		template <typename target_type>
		target_type read_value_as_(std::shared_ptr<memory::reference> data) const{
			switch (size_){
			case sizeof(unsigned __int8):
				return static_cast<target_type>(data->read_scalar<unsigned __int8>());
			case sizeof(unsigned __int16):
				return static_cast<target_type>(data->read_scalar<unsigned __int16>());
			case sizeof(unsigned __int32):
				return static_cast<target_type>(data->read_scalar<unsigned __int32>());
			case sizeof(unsigned __int64):
				return static_cast<target_type>(data->read_scalar<unsigned __int64>());
			default:
				break;
			}

			return target_type();
		}

		std::list<std::string> items_;
		std::unordered_map<std::string, std::size_t> items_map_;

		std::size_t size_ = 0u;
		std::size_t address_ = 0u;
	};
}
