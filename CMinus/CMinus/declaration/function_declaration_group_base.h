#pragma once

#include "function_declaration_base.h"

namespace cminus::declaration{
	class function_group_base{
	public:
		virtual ~function_group_base() = default;

		virtual const std::string &get_name() const = 0;

		virtual storage::object *get_parent() const = 0;

		virtual std::size_t get_address() const = 0;

		virtual std::shared_ptr<type::object> get_type() const = 0;

		virtual void add(std::shared_ptr<function_base> entry) = 0;

		virtual std::shared_ptr<function_base> find(const type::object &target_type) const = 0;

		virtual std::shared_ptr<function_base> find(const std::vector<std::shared_ptr<memory::reference>> &args, std::size_t *count = nullptr) const = 0;
	};
}
