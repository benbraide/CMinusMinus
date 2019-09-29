#pragma once

#include "../declaration/callable_declaration.h"

#include "storage_object.h"

namespace cminus::storage{
	class class_member : public unnamed_object{
	public:
		explicit class_member(const declaration::callable &owner);

		virtual ~class_member();

		virtual const std::string &get_name() const override;

		virtual object *get_parent() const override;

		virtual std::shared_ptr<memory::reference> find(const std::string &name, bool search_tree) const override;

		virtual std::shared_ptr<memory::reference> get_context() const;

		virtual const declaration::callable &get_owner() const;

	protected:
		virtual void add_(std::shared_ptr<declaration::variable> entry, std::size_t address) override;

		const declaration::callable &owner_;
		std::shared_ptr<memory::reference> context_;
	};

	class class_wrapper : public object{
	public:
		explicit class_wrapper(std::shared_ptr<memory::reference> context);

		virtual ~class_wrapper();

		virtual const std::string &get_name() const override;

		virtual std::string get_qname() const override;

		virtual object *get_parent() const override;

		virtual void add(std::shared_ptr<declaration::object> entry, std::size_t address) override;

		virtual void add(const std::string &name) override;

		virtual void add_entry(const std::string &name, std::shared_ptr<memory::reference> value, bool check_existing = true) override;

		virtual void del(const std::string &name) override;

		virtual bool exists(const std::string &name, entry_type type = entry_type::nil) const override;

		virtual std::shared_ptr<memory::reference> find(const std::string &name, bool search_tree) const override;

		virtual declaration::callable_group *find_operator(const std::string &name, bool search_tree) const override;

		virtual declaration::callable_group *find_operator(operators::id id, bool search_tree) const override;

		virtual std::shared_ptr<attribute::object> find_attribute(const std::string &name, bool search_tree) const override;

		virtual std::shared_ptr<type::object> find_type(const std::string &name, bool search_tree) const override;

		virtual object *find_storage(const std::string &name, bool search_tree) const override;

		virtual bool is_accessible(unsigned int access) const override;

		virtual std::shared_ptr<memory::reference> get_context() const;

	protected:
		std::shared_ptr<memory::reference> context_;
	};
}
