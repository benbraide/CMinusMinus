#pragma once

#include <list>
#include <mutex>
#include <string>
#include <variant>
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
		using entry_type = std::variant<
			std::size_t,
			std::shared_ptr<memory::reference>
		>;

		using resolved_declaration_type = std::variant<
			std::size_t,
			std::shared_ptr<memory::reference>,
			declaration::callable_group *,
			std::shared_ptr<attribute::object>,
			std::shared_ptr<type::object>,
			std::shared_ptr<object>
		>;

		struct entry_info{
			declaration::object *decl;
			entry_type value;
		};

		struct declaration_info{
			std::shared_ptr<declaration::object> value;
			resolved_declaration_type resolved;
		};

		virtual ~object() = default;

		virtual const std::string &get_name() const = 0;

		virtual std::string get_qname() const = 0;

		virtual object *get_parent() const = 0;

		virtual void add(std::shared_ptr<declaration::object> entry, std::size_t address) = 0;

		virtual void add(const std::string &name) = 0;

		virtual void add_entry(std::shared_ptr<declaration::object> entry, std::shared_ptr<memory::reference> value, bool check_existing = true) = 0;

		virtual bool exists(const std::string &name) const = 0;

		virtual bool exists(operators::id id) const = 0;

		virtual std::shared_ptr<memory::reference> find(const std::string &name, bool search_tree) const = 0;

		virtual std::shared_ptr<memory::reference> find(const std::string &name, std::shared_ptr<memory::reference> context, bool search_tree) const = 0;

		virtual std::shared_ptr<memory::reference> find(operators::id id, bool search_tree) const = 0;

		virtual std::shared_ptr<memory::reference> find(operators::id id, std::shared_ptr<memory::reference> context, bool search_tree) const = 0;

		virtual declaration::callable_group *find_operator(operators::id id, bool search_tree) const = 0;

		virtual std::shared_ptr<attribute::object> find_attribute(const std::string &name, bool search_tree) const = 0;

		virtual std::shared_ptr<type::object> find_type(const std::string &name, bool search_tree) const = 0;

		virtual object *find_storage(const std::string &name, bool search_tree) const = 0;

		virtual bool is_accessible(unsigned int access) const = 0;

		virtual void traverse_declarations(const std::function<void(const declaration_info &)> &callback) const = 0;

		virtual void traverse_entries(const std::function<void(const entry_info &)> &callback, bool reversed = false) const = 0;

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
		class resolved_declaration_type_visitor{
		public:
			resolved_declaration_type_visitor(const unnamed_object &target, const declaration::object &decl, std::shared_ptr<memory::reference> context, std::size_t address);

			std::shared_ptr<memory::reference> operator ()(std::size_t val);

			std::shared_ptr<memory::reference> operator ()(std::shared_ptr<memory::reference> val);

			std::shared_ptr<memory::reference> operator ()(declaration::callable_group *val);

			std::shared_ptr<memory::reference> operator ()(std::shared_ptr<attribute::object> val);

			std::shared_ptr<memory::reference> operator ()(std::shared_ptr<type::object> val);

			std::shared_ptr<memory::reference> operator ()(std::shared_ptr<object> val);

		private:
			const unnamed_object &target_;
			const declaration::object &decl_;
			std::shared_ptr<memory::reference> context_;
			std::size_t address_;
		};

		class lock_guard{
		public:
			explicit lock_guard(const unnamed_object &target);

			~lock_guard();

		private:
			const unnamed_object &target_;
		};

		virtual ~unnamed_object();

		virtual std::string get_qname() const override;

		virtual void add(std::shared_ptr<declaration::object> entry, std::size_t address) override;

		virtual void add_entry(std::shared_ptr<declaration::object> entry, std::shared_ptr<memory::reference> value, bool check_existing = true) override;

		virtual bool exists(const std::string &name) const override;

		virtual bool exists(operators::id id) const override;

		virtual std::shared_ptr<memory::reference> find(const std::string &name, bool search_tree) const override;

		virtual std::shared_ptr<memory::reference> find(const std::string &name, std::shared_ptr<memory::reference> context, bool search_tree) const override;

		virtual std::shared_ptr<memory::reference> find(operators::id id, bool search_tree) const override;

		virtual std::shared_ptr<memory::reference> find(operators::id id, std::shared_ptr<memory::reference> context, bool search_tree) const override;

		virtual declaration::callable_group *find_operator(operators::id id, bool search_tree) const override;

		virtual std::shared_ptr<attribute::object> find_attribute(const std::string &name, bool search_tree) const override;

		virtual std::shared_ptr<type::object> find_type(const std::string &name, bool search_tree) const override;

		virtual object *find_storage(const std::string &name, bool search_tree) const override;

		virtual bool is_accessible(unsigned int access) const override;

		virtual void traverse_declarations(const std::function<void(const declaration_info &)> &callback) const override;

		virtual void traverse_entries(const std::function<void(const entry_info &)> &callback, bool reversed = false) const override;

	protected:
		virtual void acquire_lock_() const;

		virtual void release_lock_() const;

		virtual void destroy_entries_();

		virtual void add_variable_(std::shared_ptr<declaration::variable> entry, std::size_t address);

		virtual std::shared_ptr<memory::reference> initialize_variable_(declaration::variable &entry, std::size_t address);

		virtual void add_callable_(std::shared_ptr<declaration::callable> entry, std::size_t address);

		virtual void add_attribute_(std::shared_ptr<attribute::object> entry);

		virtual void add_type_(std::shared_ptr<type::object> entry);

		virtual void add_storage_(std::shared_ptr<object> entry);

		virtual void add_entry_(std::shared_ptr<declaration::object> entry, std::shared_ptr<memory::reference> value, bool check_existing);

		virtual bool exists_(const std::string &name) const;

		virtual bool exists_(operators::id id) const;

		virtual std::shared_ptr<memory::reference> find_(const std::string &name, std::shared_ptr<memory::reference> context, std::size_t address) const;

		virtual std::shared_ptr<memory::reference> find_(operators::id id, std::shared_ptr<memory::reference> context) const;

		virtual declaration::callable_group *find_operator_(operators::id id) const;

		virtual std::shared_ptr<attribute::object> find_attribute_(const std::string &name) const;

		virtual std::shared_ptr<type::object> find_type_(const std::string &name) const;

		virtual object *find_storage_(const std::string &name) const;

		std::list<entry_info> entries_;
		std::unordered_map<std::string, declaration_info> declarations_;

		mutable std::mutex mutex_;
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
