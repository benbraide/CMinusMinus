#pragma once

#include "../storage/storage_object.h"
#include "../declaration/declaration_object.h"

namespace cminus::type{
	class class_ : public type::object, public storage::unnamed_object{
	public:
		using type_base = type::object;
		using storage_base = storage::unnamed_object;

		struct type_operator_info{
			std::shared_ptr<type_base> key;
			std::shared_ptr<declaration::callable_group> group;
		};

		struct base_type_info{
			unsigned int access;
			std::size_t address_offset;
			std::shared_ptr<class_> value;
		};

		struct member_variable_info{
			std::size_t address_offset;
			std::shared_ptr<declaration::variable> value;
		};

		class_(const std::string &name, storage_base *parent);

		virtual ~class_();

		virtual const std::string &get_name() const override;

		virtual std::string get_qname() const override;

		virtual storage::object *get_parent() const override;

		virtual void destruct(std::shared_ptr<memory::reference> target) const override;

		virtual std::shared_ptr<memory::reference> get_default_value() const override;

		virtual void print_value(io::writer &writer, std::shared_ptr<memory::reference> data) const override;

		virtual std::size_t get_size() const override;

		virtual std::shared_ptr<memory::reference> cast(std::shared_ptr<memory::reference> data, std::shared_ptr<type_base> target_type, cast_type type) const override;

		virtual bool is_accessible(unsigned int access) const override;

		virtual std::size_t compute_base_offset(const class_ &base_type) const;

		virtual void add_base(unsigned int access, std::shared_ptr<class_> value);

		virtual void add_default_functions(bool add_on_empty);

		using storage_base::exists;
		using storage_base::find;
		using storage_base::find_operator;

		virtual bool exists(const type::object &target_type) const;

		virtual std::shared_ptr<memory::reference> find(const std::string &name, std::shared_ptr<memory::reference> context, bool search_tree) const;

		virtual std::shared_ptr<memory::reference> find(const type::object &target_type, std::shared_ptr<memory::reference> context, bool exact = false) const;

		virtual declaration::callable *find_operator(const type::object &target_type, bool exact = false, declaration::callable_group **group = nullptr) const;

		virtual const member_variable_info *find_non_static_member(const std::string &name) const;

		virtual std::shared_ptr<memory::reference> find_non_static_member(const std::string &name, std::shared_ptr<memory::reference> context) const;

		virtual std::shared_ptr<memory::reference> find_static_member(const std::string &name) const;

		virtual declaration::callable_group *find_function(const std::string &name) const;

		virtual bool is_constructible_from(const type_base &target_type, bool is_lval, bool is_const) const;

		virtual bool is_assignable_to(std::shared_ptr<type_base> target_type) const;

		virtual bool is_base_type(const class_ &target, bool search_hierarchy) const;

		virtual const std::list<base_type_info> &get_base_types() const;

		virtual void compile();

	protected:
		virtual void construct_(std::shared_ptr<memory::reference> target, const std::vector<std::shared_ptr<memory::reference>> &args) const override;

		virtual int get_score_(const type_base &target, bool is_lval, bool is_const) const override;

		virtual int get_no_conversion_score_(const type_base &target, bool is_lval, bool is_const) const override;

		using storage_base::exists_;
		using storage_base::find_;
		using storage_base::find_operator_;

		virtual void acquire_lock_() const override;

		virtual void release_lock_() const override;

		virtual void add_variable_(std::shared_ptr<declaration::variable> entry, std::size_t address) override;

		virtual void add_callable_(std::shared_ptr<declaration::callable> entry, std::size_t address) override;

		virtual bool exists_(const type::object &target_type) const;

		virtual std::shared_ptr<memory::reference> find_(const std::string &name, std::shared_ptr<memory::reference> context, std::size_t address) const override;

		virtual std::shared_ptr<memory::reference> find_(const type::object &target_type, std::shared_ptr<memory::reference> context, bool exact) const;

		virtual declaration::callable *find_operator_(const type::object &target_type, bool exact, declaration::callable_group **group) const;

		virtual storage::object *find_storage_(const std::string &name) const override;

		virtual class_ *find_base_type_(const std::string &name, bool search_hierarchy) const;

		virtual std::size_t compute_base_offset_(const class_ &base_type, std::size_t offset) const;

		virtual bool is_base_type_(const class_ &target, bool search_hierarchy) const;

		virtual unsigned int get_base_type_access_(const class_ &target, bool skip_immediate) const;

		virtual declaration::callable_group *find_function_(const std::string &name) const;

		virtual void compile_();

		std::list<base_type_info> base_types_;

		std::size_t size_ = 0u;
		std::size_t member_size_ = 0u;

		std::size_t static_address_ = 0u;
		std::size_t static_size_ = 0u;

		std::size_t function_address_ = 0u;
		std::size_t function_size_ = 0u;

		std::shared_ptr<memory::reference> class_context_;
		std::shared_ptr<memory::reference> dummy_context_;
	};
}
