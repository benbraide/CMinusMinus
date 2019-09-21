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
	class function_base : public object{
	public:
		virtual ~function_base() = default;

		virtual storage::object *get_parent() const = 0;

		virtual std::shared_ptr<type::object> get_type() const = 0;

		virtual void define(std::shared_ptr<node::object> body) = 0;

		virtual std::shared_ptr<node::object> get_body() const = 0;

		virtual bool is_defined() const = 0;

		virtual bool is_operator() const = 0;
	};
}
