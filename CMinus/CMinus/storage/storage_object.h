#pragma once

#include <list>
#include <mutex>
#include <string>
#include <unordered_map>

#include "../type/type_object.h"
#include "../memory/memory_reference.h"
#include "../runtime/runtime_object.h"

#include "../operators/operator_conversions.h"
#include "../declaration/callable_declaration_group.h"

#include "storage_exception.h"

namespace cminus::declaration{
	class variable;
	class operator_;
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

		virtual ~object() = default;

		virtual const std::string &get_name() const = 0;

		virtual std::string get_qname() const = 0;

		virtual object *get_parent() const = 0;

		virtual void add(std::shared_ptr<declaration::object> entry, std::size_t address) = 0;

		virtual void add(const std::string &name) = 0;

		virtual void add_entry(const std::string &name, std::shared_ptr<memory::reference> value, bool check_existing = true) = 0;

		virtual void del(const std::string &name) = 0;

		virtual bool exists(const std::string &name, entry_type type = entry_type::nil) const = 0;

		virtual bool exists(operators::id id) const = 0;

		virtual std::shared_ptr<memory::reference> find(const std::string &name, bool search_tree) const = 0;

		virtual std::shared_ptr<memory::reference> find(operators::id id, bool search_tree) const = 0;

		virtual declaration::callable_group *find_operator(operators::id id, bool search_tree) const = 0;

		virtual std::shared_ptr<attribute::object> find_attribute(const std::string &name, bool search_tree) const = 0;

		virtual std::shared_ptr<type::object> find_type(const std::string &name, bool search_tree) const = 0;

		virtual object *find_storage(const std::string &name, bool search_tree) const = 0;

		virtual bool is_accessible(unsigned int access) const = 0;

		template <typename target_type>
		target_type *get_first_of() const{
			if (auto self_target = dynamic_cast<const target_type *>(this); self_target != nullptr)
				return const_cast<target_type *>(self_target);
			
			if (auto parent = get_parent(); parent != nullptr)
				return parent->get_first_of<target_type>();

			return nullptr;
		}
	};

	class unnamed_object : public object{
	public:
		virtual ~unnamed_object();

		virtual std::string get_qname() const override;

		virtual void add(std::shared_ptr<declaration::object> entry, std::size_t address) override;

		virtual void add(const std::string &name) override;

		virtual void add_entry(const std::string &name, std::shared_ptr<memory::reference> value, bool check_existing = true) override;

		virtual void del(const std::string &name) override;

		virtual bool exists(const std::string &name, entry_type type = entry_type::nil) const override;

		virtual bool exists(operators::id id) const override;

		virtual std::shared_ptr<memory::reference> find(const std::string &name, bool search_tree) const override;

		virtual std::shared_ptr<memory::reference> find(operators::id id, bool search_tree) const override;

		virtual declaration::callable_group *find_operator(operators::id id, bool search_tree) const override;

		virtual std::shared_ptr<attribute::object> find_attribute(const std::string &name, bool search_tree) const override;

		virtual std::shared_ptr<type::object> find_type(const std::string &name, bool search_tree) const override;

		virtual object *find_storage(const std::string &name, bool search_tree) const override;

		virtual bool is_accessible(unsigned int access) const override;

	protected:
		virtual void destroy_entries_();

		virtual bool add_(std::shared_ptr<declaration::object> entry, std::size_t address);

		virtual void add_variable_(std::shared_ptr<declaration::variable> entry, std::size_t address);

		virtual void add_callable_(std::shared_ptr<declaration::callable> entry, std::size_t address);

		virtual void add_operator_(std::shared_ptr<declaration::operator_> entry, std::size_t address);

		virtual void add_attribute_(std::shared_ptr<attribute::object> entry);

		virtual void add_type_(std::shared_ptr<type::object> entry);

		virtual void add_storage_(std::shared_ptr<object> entry);

		virtual void del_(const std::string &name);

		virtual bool exists_(const std::string &name, entry_type type) const;

		virtual bool exists_(operators::id id) const;

		virtual std::shared_ptr<memory::reference> find_(const std::string &name) const;

		virtual std::shared_ptr<memory::reference> find_(operators::id id) const;

		virtual declaration::callable_group *find_operator_(operators::id id) const;

		virtual std::shared_ptr<attribute::object> find_attribute_(const std::string &name) const;

		virtual std::shared_ptr<type::object> find_type_(const std::string &name) const;

		virtual object *find_storage_(const std::string &name) const;

		std::list<std::shared_ptr<memory::reference>> entries_;
		std::unordered_map<std::string, std::shared_ptr<memory::reference>> named_entries_;

		std::unordered_map<std::string, std::shared_ptr<declaration::callable_group>> functions_;
		std::unordered_map<operators::id, std::shared_ptr<declaration::callable_group>> operators_;

		std::unordered_map<std::string, std::shared_ptr<declaration::object>> declarations_;
		std::unordered_map<std::string, std::shared_ptr<attribute::object>> attributes_;

		std::unordered_map<std::string, std::shared_ptr<type::object>> types_;
		std::unordered_map<std::string, std::shared_ptr<object>> storages_;

		mutable std::mutex lock_;
	};

	class named_object : public unnamed_object{
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
