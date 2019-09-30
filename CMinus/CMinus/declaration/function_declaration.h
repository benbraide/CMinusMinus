#pragma once

#include "../type/function_type.h"
#include "../attribute/attribute_collection.h"

#include "callable_declaration.h"

namespace cminus::declaration{
	class function : public callable{
	public:
		using parameter_list_type = std::list<std::shared_ptr<variable>>;

		template <typename attributes_type>
		function(const std::string &name, storage::object *parent, const attributes_type &attributes, unsigned int flags, std::shared_ptr<type::object> return_type)
			: callable(name, nullptr, attributes, (flags & ~declaration::flags::tls)), parent_(parent){
			init_(return_type);
		}

		virtual ~function();

		virtual id_type get_id() const override;

		virtual storage::object *get_parent() const override;

		virtual std::shared_ptr<type::object> get_type() const override;

		virtual void add_parameter(std::shared_ptr<variable> value) override;

		virtual bool is_defined() const override;

		virtual int get_score(std::shared_ptr<memory::reference> context, const std::list<std::shared_ptr<memory::reference>> &args) const override;

	protected:
		virtual std::shared_ptr<memory::reference> call_(std::shared_ptr<memory::reference> context, const std::list<std::shared_ptr<memory::reference>> &args) const override;

		virtual void init_(std::shared_ptr<type::object> return_type);

		virtual int get_context_score_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const;

		virtual void copy_context_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const;

		virtual void copy_args_(std::shared_ptr<memory::reference> context, const std::list<std::shared_ptr<memory::reference>> &args) const;

		virtual void evaluate_body_() const;

		virtual std::shared_ptr<memory::reference> copy_return_value_(std::shared_ptr<memory::reference> value) const;

		virtual int get_arg_score_(std::shared_ptr<type::object> param_type, std::shared_ptr<memory::reference> arg) const;

		virtual std::size_t get_args_count_(std::shared_ptr<memory::reference> context, const std::list<std::shared_ptr<memory::reference>> &args) const;

		storage::object *parent_;
		std::shared_ptr<type::function> type_;

		std::shared_ptr<variable> return_declaration_;
		parameter_list_type parameter_list_;

		std::size_t min_arg_count_ = 0u;
		std::size_t max_arg_count_ = 0u;
	};

	class defined_function : public function{
	public:
		using function::function;

		virtual ~defined_function();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

	protected:
		std::shared_ptr<node::object> definition_;
	};

	class external_function : public function{
	public:
		using function::function;

		virtual ~external_function();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

		virtual bool is_defined() const override;
	};
}
