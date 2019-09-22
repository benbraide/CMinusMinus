#pragma once

#include <list>
#include <mutex>
#include <string>
#include <unordered_map>

#include "../type/type_object.h"
#include "../memory/memory_reference.h"
#include "../runtime/runtime_object.h"

#include "../declaration/declaration_object.h"

#include "storage_exception.h"

namespace cminus::declaration{
	class variable;
}

namespace cminus::storage{
	class object{
	public:
		enum class entry_type{
			nil,
			mem_ref,
			function,
			attr,
			type,
			storage,
		};

		virtual ~object();

		virtual const std::string &get_name() const = 0;

		virtual std::string get_qname() const;

		virtual object *get_parent() const = 0;

		virtual void add(std::shared_ptr<declaration::object> entry, std::size_t address);

		virtual void add(const std::string &name);

		virtual void del(const std::string &name);

		virtual bool exists(const std::string &name, entry_type type = entry_type::nil) const;

		virtual std::shared_ptr<memory::reference> find(const std::string &name, bool search_tree) const;

		virtual std::shared_ptr<attribute::object> find_attribute(const std::string &name, bool search_tree) const;

		virtual std::shared_ptr<type::object> find_type(const std::string &name, bool search_tree) const;

		virtual std::shared_ptr<object> find_storage(const std::string &name, bool search_tree) const;

		template <typename target_type>
		target_type *get_first_of() const{
			if (auto self_target = dynamic_cast<const target_type *>(this); self_target != nullptr)
				return const_cast<target_type *>(self_target);
			
			if (auto parent = get_parent(); parent != nullptr)
				return parent->get_first_of();

			return nullptr;
		}

	protected:
		virtual void destroy_entries_();

		virtual void add_(std::shared_ptr<declaration::variable> entry, std::size_t address);

		virtual void add_(std::shared_ptr<declaration::function_base> entry, std::size_t address);

		virtual void add_(std::shared_ptr<attribute::object> entry);

		virtual void add_(std::shared_ptr<type::object> entry);

		virtual void add_(std::shared_ptr<object> entry);

		virtual void del_(const std::string &name);

		virtual bool exists_(const std::string &name, entry_type type) const;

		virtual std::shared_ptr<memory::reference> find_(const std::string &name) const;

		virtual std::shared_ptr<attribute::object> find_attribute_(const std::string &name) const;
		
		virtual std::shared_ptr<type::object> find_type_(const std::string &name) const;

		virtual std::shared_ptr<object> find_storage_(const std::string &name) const;

		std::list<std::shared_ptr<memory::reference>> entries_;
		std::unordered_map<std::string, std::shared_ptr<memory::reference>> named_entries_;

		std::unordered_map<std::string, std::shared_ptr<declaration::function_group_base>> functions_;
		std::unordered_map<std::string, std::shared_ptr<attribute::object>> attributes_;

		std::unordered_map<std::string, std::shared_ptr<type::object>> types_;
		std::unordered_map<std::string, std::shared_ptr<object>> storages_;

		mutable std::mutex lock_;
	};

	class named_object : public object{
	public:
		named_object(const std::string &name, object *parent);

		virtual ~named_object();

		virtual const std::string &get_name() const override;

		virtual object *get_parent() const override;

	protected:
		std::string name_;
		object *parent_;
	};
}
