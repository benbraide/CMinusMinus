#pragma once

#include "../type/class_type.h"

#include "function_declaration.h"

namespace cminus::declaration{
	class constructor : public function{
	public:
		struct init_info{
			std::shared_ptr<node::object> key;
			std::shared_ptr<node::object> initialization;
		};

		explicit constructor(type::class_ &parent);

		virtual ~constructor();

		virtual void add_init(std::shared_ptr<node::object> key, std::shared_ptr<node::object> initialization);

	protected:
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
	};

	class default_constructor : public external_constructor{
	public:
		explicit default_constructor(type::class_ &parent);

		virtual ~default_constructor();

		virtual void add_parameter(std::shared_ptr<variable> value) override;

		virtual bool is_defined() const override;

		virtual void add_init(std::shared_ptr<node::object> key, std::shared_ptr<node::object> initialization);

	protected:
		virtual void evaluate_body_() const override;
	};

	class copy_constructor : public external_constructor{
	public:
		explicit copy_constructor(type::class_ &parent);

		virtual ~copy_constructor();

		virtual void add_parameter(std::shared_ptr<variable> value) override;

		virtual bool is_defined() const override;

		virtual void add_init(std::shared_ptr<node::object> key, std::shared_ptr<node::object> initialization);

	protected:
		virtual void evaluate_body_() const override;
	};

	class destructor : public function{
	public:
		explicit destructor(type::class_ &parent);

		virtual ~destructor();

		virtual void add_parameter(std::shared_ptr<variable> value) override;

	protected:
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
	};

	class default_destructor : public external_destructor{
	public:
		explicit default_destructor(type::class_ &parent);

		virtual ~default_destructor();

		virtual bool is_defined() const override;
	};
}
