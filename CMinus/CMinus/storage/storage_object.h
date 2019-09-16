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

namespace cminus::storage{
	class object{
	public:
		struct search_options{
			const object *scope;
			std::shared_ptr<memory::reference> context;
			std::string name;
			bool search_tree;
			const object **branch;
		};

		struct unnamed_search_options{
			const object *scope;
			std::shared_ptr<memory::reference> context;
			bool search_tree;
			const object **branch;
		};

		enum class entry_type{
			nil,
			mem_ref,
			function,
			attr,
			type,
			storage,
		};

		object(const std::string &name, object *parent);

		virtual ~object();

		virtual const std::string &get_name() const;

		virtual std::string get_qname() const;

		virtual object *get_parent() const;

		virtual void add(std::shared_ptr<declaration::object> entry, std::size_t address);

		virtual void add(const std::string &name);

		virtual void del(const std::string &name);

		virtual bool exists(const std::string &name, entry_type type = entry_type::nil) const;

		virtual std::shared_ptr<memory::reference> find(const std::string &name, bool search_tree) const;

		virtual std::shared_ptr<attribute::object> find_attribute(const std::string &name, bool search_tree) const;

		virtual std::shared_ptr<type::object> find_type(const std::string &name, bool search_tree) const;

		virtual std::shared_ptr<object> find_storage(const std::string &name, bool search_tree) const;

	protected:
		virtual void destroy_entries_();

		virtual void add_(const std::string &name, std::shared_ptr<memory::reference> entry);

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

		std::string name_;
		object *parent_;

		std::list<std::shared_ptr<memory::reference>> entries_;
		std::unordered_map<std::string, memory::reference *> named_entries_;

		std::unordered_map<std::string, std::shared_ptr<declaration::function_group_base>> functions_;
		std::unordered_map<std::string, std::shared_ptr<attribute::object>> attributes_;

		std::unordered_map<std::string, std::shared_ptr<type::object>> types_;
		std::unordered_map<std::string, std::shared_ptr<object>> storages_;

		mutable std::mutex lock_;
	};
}
