#pragma once

#include "../type/type_object.h"

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
			constructor,
			destructor,
		};

		virtual ~callable();

		virtual id_type get_id() const = 0;

		virtual storage::object *get_parent() const = 0;

		virtual std::shared_ptr<type::object> get_type() const = 0;

		virtual void add_parameter(std::shared_ptr<variable> value) = 0;

		virtual void define(std::shared_ptr<node::object> definition) = 0;

		virtual std::shared_ptr<node::object> get_definition() const = 0;

		virtual bool is_defined() const = 0;

		virtual int get_score(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args) const = 0;

		virtual std::shared_ptr<memory::reference> call(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args) const;

	protected:
		friend class callable_group;

		virtual std::shared_ptr<memory::reference> call_(std::shared_ptr<memory::reference> context, const std::vector<std::shared_ptr<memory::reference>> &args) const = 0;
	};
}
