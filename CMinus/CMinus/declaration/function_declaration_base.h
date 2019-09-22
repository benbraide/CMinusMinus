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
	class function_group;

	class function_base : public object{
	public:
		using object::object;

		virtual ~function_base();

		virtual storage::object *get_parent() const = 0;

		virtual std::shared_ptr<type::object> get_type() const = 0;

		virtual void add_parameter(std::shared_ptr<object> value) = 0;

		virtual void define(std::shared_ptr<node::object> definition) = 0;

		virtual std::shared_ptr<node::object> get_definition() const = 0;

		virtual bool is_defined() const = 0;

		virtual int get_score(const std::vector<std::shared_ptr<memory::reference>> &args) const = 0;

		virtual std::shared_ptr<memory::reference> call(const std::vector<std::shared_ptr<memory::reference>> &args) const;

	protected:
		friend class function_group;

		virtual std::shared_ptr<memory::reference> call_(const std::vector<std::shared_ptr<memory::reference>> &args) const = 0;
	};
}
