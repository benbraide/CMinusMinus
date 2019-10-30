#pragma once

#include "../type/function_type.h"

#include "declaration_object.h"

namespace cminus::memory{
	class reference;
}

namespace cminus::storage{
	class object;
}

namespace cminus::node{
	class object;
}

namespace cminus::declaration{
	class variable;
	class callable_group;

	class callable : public object{
	public:
		using object::object;

		enum class id_type{
			function,
			member,
			operator_,
			type_operator,
			constructor,
			destructor,
		};

		struct arg_info{
			const type::object *type;
			bool is_lval;
			bool is_const;
		};

		virtual ~callable();

		virtual id_type get_id() const = 0;

		virtual storage::object *get_parent() const = 0;

		virtual void add_parameter(std::shared_ptr<variable> value) = 0;

		virtual void define(std::shared_ptr<node::object> definition) = 0;

		virtual std::shared_ptr<node::object> get_definition() const = 0;

		virtual bool is_defined() const = 0;

		virtual int get_score(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const = 0;

		virtual int get_score(std::shared_ptr<memory::reference> context, const std::vector<arg_info> &args, std::size_t required_size) const = 0;

		virtual int get_args_score(const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const = 0;

		virtual int get_args_score(const std::vector<arg_info> &args, std::size_t required_size) const = 0;

		virtual std::shared_ptr<memory::reference> call(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const;

	protected:
		friend class callable_group;

		virtual std::shared_ptr<memory::reference> call_(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t required_size) const = 0;
	};
}
