#pragma once

#include "../type/class_type.h"
#include "../type/proxy_type.h"
#include "../type/modified_type.h"

#include "variable_declaration.h"
#include "function_declaration.h"

namespace cminus::declaration{
	class member_function : public function{
	public:
		template <typename attributes_type>
		member_function(const std::string &name, type::class_ &parent, const attributes_type &attributes, unsigned int flags, std::shared_ptr<type::object> return_type)
			: function(name, &parent, attributes, flags, return_type){
			init_context_(parent);
		}

		virtual ~member_function();

		virtual id_type get_id() const override;

	protected:
		virtual int get_context_score_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const override;

		virtual void copy_context_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const override;

		virtual std::size_t get_args_count_(std::shared_ptr<memory::reference> context, std::size_t args_count) const override;

		virtual void init_context_(type::class_ &parent);

		std::shared_ptr<variable> context_declaration_;
	};

	class defined_member_function : public member_function{
	public:
		using member_function::member_function;

		virtual ~defined_member_function();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

	protected:
		std::shared_ptr<node::object> definition_;
	};

	class external_member_function : public member_function{
	public:
		using member_function::member_function;

		virtual ~external_member_function();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

		virtual bool is_defined() const override;
	};

	class constructor : public member_function{
	public:
		struct init_info{
			std::shared_ptr<node::object> key;
			std::shared_ptr<node::object> initialization;
		};

		template <typename attributes_type>
		constructor(type::class_ &parent, const attributes_type &attributes, unsigned int flags)
			: member_function(parent.get_name(), parent, attributes, (flags & ~(declaration::flags::const_ | declaration::flags::static_)), nullptr){}

		virtual ~constructor();

		virtual id_type get_id() const override;

		virtual void add_init(std::shared_ptr<node::object> key, std::shared_ptr<node::object> initialization);

	protected:
		virtual void copy_context_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const override;

		virtual void evaluate_body_() const override;

		std::list<init_info> init_list_;
	};

	class defined_constructor : public constructor{
	public:
		using constructor::constructor;

		virtual ~defined_constructor();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

	protected:
		std::shared_ptr<node::object> definition_;
	};

	class external_constructor : public constructor{
	public:
		using constructor::constructor;

		virtual ~external_constructor();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

		virtual bool is_defined() const override;
	};

	class default_constructor : public external_constructor{
	public:
		using external_constructor::external_constructor;

		virtual ~default_constructor();
	};

	class copy_constructor : public external_constructor{
	public:
		template <typename attributes_type>
		explicit copy_constructor(type::class_ &parent, const attributes_type &attributes, unsigned int flags)
			: external_constructor(parent, attributes, flags){
			std::shared_ptr<type::object> other_type = std::make_shared<type::proxy>(parent);

			other_type = std::make_shared<type::ref>(other_type);
			other_type = std::make_shared<type::constant>(other_type);

			std::shared_ptr<memory::reference> empty_initialization;
			add_parameter(std::make_shared<variable>(
				"other",								//Name
				other_type,								//Type
				attribute::collection::list_type{},		//Attributes
				flags::nil,								//Flags
				empty_initialization					//Initialization
			));
		}

		virtual ~copy_constructor();

	protected:
		virtual void evaluate_body_() const override;
	};

	class destructor : public member_function{
	public:
		template <typename attributes_type>
		explicit destructor(type::class_ &parent, const attributes_type &attributes, unsigned int flags)
			: member_function(("~" + parent.get_name()), parent, attributes, (flags & ~(declaration::flags::const_ | declaration::flags::static_)), nullptr){}

		virtual ~destructor();

		virtual id_type get_id() const override;

	protected:
		virtual void copy_context_(std::shared_ptr<memory::reference> context, parameter_list_type::const_iterator &it) const override;

		virtual void evaluate_body_() const override;
	};

	class defined_destructor : public destructor{
	public:
		using destructor::destructor;

		virtual ~defined_destructor();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

	protected:
		std::shared_ptr<node::object> definition_;
	};

	class external_destructor : public destructor{
	public:
		using destructor::destructor;

		virtual ~external_destructor();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

		virtual bool is_defined() const override;
	};

	class default_destructor : public external_destructor{
	public:
		using external_destructor::external_destructor;

		virtual ~default_destructor();
	};

	class operator_ : public function{
	public:
		template <typename attributes_type>
		operator_(operators::id id, type::class_ &parent, const attributes_type &attributes, unsigned int flags, std::shared_ptr<type::object> return_type)
			: function(operators::convert_id_to_string(id), &parent, attributes, flags, return_type), id_(id){}

		virtual ~operator_();

		virtual id_type get_id() const override;

		virtual operators::id get_op() const;

	protected:
		operators::id id_;
	};

	class defined_operator : public operator_{
	public:
		using operator_::operator_;

		virtual ~defined_operator();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

	protected:
		std::shared_ptr<node::object> definition_;
	};

	class external_operator : public operator_{
	public:
		using operator_::operator_;

		virtual ~external_operator();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

		virtual bool is_defined() const override;
	};

	class member_operator : public member_function{
	public:
		template <typename attributes_type>
		member_operator(operators::id id, type::class_ &parent, const attributes_type &attributes, unsigned int flags, std::shared_ptr<type::object> return_type)
			: member_function(operators::convert_id_to_string(id), parent, attributes, flags, return_type), id_(id){}

		virtual ~member_operator();

		virtual id_type get_id() const override;

		virtual operators::id get_op() const;

	protected:
		operators::id id_;
	};

	class defined_member_operator : public member_operator{
	public:
		using member_operator::member_operator;

		virtual ~defined_member_operator();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

	protected:
		std::shared_ptr<node::object> definition_;
	};

	class external_member_operator : public member_operator{
	public:
		using member_operator::member_operator;

		virtual ~external_member_operator();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

		virtual bool is_defined() const override;
	};

	class type_operator : public member_function{
	public:
		template <typename attributes_type>
		type_operator(std::shared_ptr<type::object> target_type, type::class_ &parent, const attributes_type &attributes, unsigned int flags)
			: member_function("#TypeOperator#", parent, attributes, flags, target_type_), target_type_(target_type){
			insert_target_type_();
		}

		virtual ~type_operator();

		virtual id_type get_id() const override;

		virtual std::shared_ptr<type::object> get_target_type() const;

	protected:
		virtual void insert_target_type_();

		std::shared_ptr<type::object> target_type_;
	};

	class defined_type_operator : public type_operator{
	public:
		using type_operator::type_operator;

		virtual ~defined_type_operator();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

	protected:
		std::shared_ptr<node::object> definition_;
	};

	class external_type_operator : public type_operator{
	public:
		using type_operator::type_operator;

		virtual ~external_type_operator();

		virtual void define(std::shared_ptr<node::object> definition) override;

		virtual std::shared_ptr<node::object> get_definition() const override;

		virtual bool is_defined() const override;
	};
}
